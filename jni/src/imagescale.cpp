#include "imagescale.h"
#include <cstring>


struct QImageScaleInfo {
    int *xpoints;
    unsigned int **ypoints;
    int *xapoints, *yapoints;
    int xup_yup;
};


template <typename T>
inline const T &qMin(const T &a, const T &b) { return (a < b) ? a : b; }

template <typename T>
inline const T &qMax(const T &a, const T &b) { return (a < b) ? b : a; }

template <typename T>
inline T qAbs(const T &t) { return t >= 0 ? t : -t; }

typedef unsigned int QRgb;                        // RGB triplet

inline int qRed(QRgb rgb)                // get red part of RGB
{ return ((rgb >> 16) & 0xff); }

inline int qGreen(QRgb rgb)                // get green part of RGB
{ return ((rgb >> 8) & 0xff); }

inline int qBlue(QRgb rgb)                // get blue part of RGB
{ return (rgb & 0xff); }

inline int qAlpha(QRgb rgb)                // get alpha part of RGBA
{ return rgb >> 24; }

inline QRgb qRgb(int r, int g, int b)// set RGB value
{ return (0xffu << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }

inline QRgb qRgba(int r, int g, int b, int a)// set RGBA value
{ return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }


//
// Code ported from Imlib...
//

// FIXME: replace with qRed, etc... These work on pointers to pixels, not
// pixel values
#define A_VAL(p) (qAlpha(*p))
#define R_VAL(p) (qRed(*p))
#define G_VAL(p) (qGreen(*p))
#define B_VAL(p) (qBlue(*p))

#define INV_XAP                   (256 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (256 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])

static unsigned int** qimageCalcYPoints(unsigned int *src, int sw, int sh, int dh)
{
    unsigned int **p;
    int i, j = 0;
    int val, inc, rv = 0;

    if(dh < 0){
        dh = -dh;
        rv = 1;
    }
    p = new unsigned int* [dh+1];

    int up = qAbs(dh) >= sh;
    val = up ? 0x8000 * sh / dh - 0x8000 : 0;
    inc = (sh << 16) / dh;
    for(i = 0; i < dh; i++){
        p[j++] = src + qMax(0, val >> 16) * sw;
        val += inc;
    }
    if(rv){
        for(i = dh / 2; --i >= 0; ){
            unsigned int *tmp = p[i];
            p[i] = p[dh - i - 1];
            p[dh - i - 1] = tmp;
        }
    }
    return(p);
}

int* qimageCalcXPoints(int sw, int dw)
{
    int *p, i, j = 0;
    int val, inc, rv = 0;

    if(dw < 0){
        dw = -dw;
        rv = 1;
    }
    p = new int[dw+1];

    int up = qAbs(dw) >= sw;
    val = up ? 0x8000 * sw / dw - 0x8000 : 0;
    inc = (sw << 16) / dw;
    for(i = 0; i < dw; i++){
        p[j++] = qMax(0, val >> 16);
        val += inc;
    }

    if(rv){
        for(i = dw / 2; --i >= 0; ){
            int tmp = p[i];
            p[i] = p[dw - i - 1];
            p[dw - i - 1] = tmp;
        }
    }
   return(p);
}

int* qimageCalcApoints(int s, int d, int up)
{
    int *p, i, j = 0, rv = 0;

    if(d < 0){
        rv = 1;
        d = -d;
    }
    p = new int[d];

    /* scaling up */
    if(up){
        int val, inc;

        val = 0x8000 * s / d - 0x8000;
        inc = (s << 16) / d;
        for(i = 0; i < d; i++){
            int pos = val >> 16;
            if (pos < 0)
                p[j++] = 0;
            else if (pos >= (s - 1))
                p[j++] = 0;
            else
                p[j++] = (val >> 8) - ((val >> 8) & 0xffffff00);
            val += inc;
        }
    }
    /* scaling down */
    else{
        int val, inc, ap, Cp;
        val = 0;
        inc = (s << 16) / d;
        Cp = ((d << 14) / s) + 1;
        for(i = 0; i < d; i++){
            ap = ((0x100 - ((val >> 8) & 0xff)) * Cp) >> 8;
            p[j] = ap | (Cp << 16);
            j++;
            val += inc;
        }
    }
    if(rv){
        int tmp;
        for(i = d / 2; --i >= 0; ){
            tmp = p[i];
            p[i] = p[d - i - 1];
            p[d - i - 1] = tmp;
        }
    }
    return(p);
}



/* FIXME: NEED to optimize ScaleAARGBA - currently its "ok" but needs work*/

/* scale by area sampling */
static void qt_qimageScaleAARGBA(QImageScaleInfo *isi, unsigned int *dest,
                                 int dxx, int dyy, int dx, int dy, int dw,
                                 int dh, int dow, int sow)
{
    unsigned int *sptr, *dptr;
    int x, y, end;
    unsigned int **ypoints = isi->ypoints;
    int *xpoints = isi->xpoints;
    int *xapoints = isi->xapoints;
    int *yapoints = isi->yapoints;

    end = dxx + dw;
    /* scaling up both ways */
    if(isi->xup_yup == 3){
        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            /* calculate the source line we'll scan from */
            dptr = dest + dx + ((y + dy) * dow);
            sptr = ypoints[dyy + y];
            if(YAP > 0){
                for(x = dxx; x < end; x++){
                    int r, g, b, a;
                    int rr, gg, bb, aa;
                    unsigned int *pix;

                    if(XAP > 0){
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_XAP;
                        g = G_VAL(pix) * INV_XAP;
                        b = B_VAL(pix) * INV_XAP;
                        a = A_VAL(pix) * INV_XAP;
                        pix++;
                        r += R_VAL(pix) * XAP;
                        g += G_VAL(pix) * XAP;
                        b += B_VAL(pix) * XAP;
                        a += A_VAL(pix) * XAP;
                        pix += sow;
                        rr = R_VAL(pix) * XAP;
                        gg = G_VAL(pix) * XAP;
                        bb = B_VAL(pix) * XAP;
                        aa = A_VAL(pix) * XAP;
                        pix--;
                        rr += R_VAL(pix) * INV_XAP;
                        gg += G_VAL(pix) * INV_XAP;
                        bb += B_VAL(pix) * INV_XAP;
                        aa += A_VAL(pix) * INV_XAP;
                        r = ((rr * YAP) + (r * INV_YAP)) >> 16;
                        g = ((gg * YAP) + (g * INV_YAP)) >> 16;
                        b = ((bb * YAP) + (b * INV_YAP)) >> 16;
                        a = ((aa * YAP) + (a * INV_YAP)) >> 16;
                        *dptr++ = qRgba(r, g, b, a);
                    }
                    else{
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_YAP;
                        g = G_VAL(pix) * INV_YAP;
                        b = B_VAL(pix) * INV_YAP;
                        a = A_VAL(pix) * INV_YAP;
                        pix += sow;
                        r += R_VAL(pix) * YAP;
                        g += G_VAL(pix) * YAP;
                        b += B_VAL(pix) * YAP;
                        a += A_VAL(pix) * YAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        a >>= 8;
                        *dptr++ = qRgba(r, g, b, a);
                    }
                }
            }
            else{
                for(x = dxx; x < end; x++){
                    int r, g, b, a;
                    unsigned int *pix;

                    if(XAP > 0){
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_XAP;
                        g = G_VAL(pix) * INV_XAP;
                        b = B_VAL(pix) * INV_XAP;
                        a = A_VAL(pix) * INV_XAP;
                        pix++;
                        r += R_VAL(pix) * XAP;
                        g += G_VAL(pix) * XAP;
                        b += B_VAL(pix) * XAP;
                        a += A_VAL(pix) * XAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        a >>= 8;
                        *dptr++ = qRgba(r, g, b, a);
                    }
                    else
                        *dptr++ = sptr[xpoints[x] ];
                }
            }
        }
    }
    /* if we're scaling down vertically */
    else if(isi->xup_yup == 1){
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cy, j;
        unsigned int *pix;
        int r, g, b, a, rr, gg, bb, aa;
        int yap;

        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                pix = ypoints[dyy + y] + xpoints[x];
                r = R_VAL(pix) * yap;
                g = G_VAL(pix) * yap;
                b = B_VAL(pix) * yap;
                a = A_VAL(pix) * yap;
                for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                    pix += sow;
                    r += R_VAL(pix) * Cy;
                    g += G_VAL(pix) * Cy;
                    b += B_VAL(pix) * Cy;
                    a += A_VAL(pix) * Cy;
                }
                if(j > 0){
                    pix += sow;
                    r += R_VAL(pix) * j;
                    g += G_VAL(pix) * j;
                    b += B_VAL(pix) * j;
                    a += A_VAL(pix) * j;
                }
                if(XAP > 0){
                    pix = ypoints[dyy + y] + xpoints[x] + 1;
                    rr = R_VAL(pix) * yap;
                    gg = G_VAL(pix) * yap;
                    bb = B_VAL(pix) * yap;
                    aa = A_VAL(pix) * yap;
                    for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                        pix += sow;
                        rr += R_VAL(pix) * Cy;
                        gg += G_VAL(pix) * Cy;
                        bb += B_VAL(pix) * Cy;
                        aa += A_VAL(pix) * Cy;
                    }
                    if(j > 0){
                        pix += sow;
                        rr += R_VAL(pix) * j;
                        gg += G_VAL(pix) * j;
                        bb += B_VAL(pix) * j;
                        aa += A_VAL(pix) * j;
                    }
                    r = r * INV_XAP;
                    g = g * INV_XAP;
                    b = b * INV_XAP;
                    a = a * INV_XAP;
                    r = (r + ((rr * XAP))) >> 12;
                    g = (g + ((gg * XAP))) >> 12;
                    b = (b + ((bb * XAP))) >> 12;
                    a = (a + ((aa * XAP))) >> 12;
                }
                else{
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                    a >>= 4;
                }
                *dptr = qRgba(r >> 10, g >> 10, b >> 10, a >> 10);
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally */
    else if(isi->xup_yup == 2){
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, j;
        unsigned int *pix;
        int r, g, b, a, rr, gg, bb, aa;
        int xap;

        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                pix = ypoints[dyy + y] + xpoints[x];
                r = R_VAL(pix) * xap;
                g = G_VAL(pix) * xap;
                b = B_VAL(pix) * xap;
                a = A_VAL(pix) * xap;
                for(j = (1 << 14) - xap; j > Cx; j -= Cx){
                    pix++;
                    r += R_VAL(pix) * Cx;
                    g += G_VAL(pix) * Cx;
                    b += B_VAL(pix) * Cx;
                    a += A_VAL(pix) * Cx;
                }
                if(j > 0){
                    pix++;
                    r += R_VAL(pix) * j;
                    g += G_VAL(pix) * j;
                    b += B_VAL(pix) * j;
                    a += A_VAL(pix) * j;
                }
                if(YAP > 0){
                    pix = ypoints[dyy + y] + xpoints[x] + sow;
                    rr = R_VAL(pix) * xap;
                    gg = G_VAL(pix) * xap;
                    bb = B_VAL(pix) * xap;
                    aa = A_VAL(pix) * xap;
                    for(j = (1 << 14) - xap; j > Cx; j -= Cx){
                        pix++;
                        rr += R_VAL(pix) * Cx;
                        gg += G_VAL(pix) * Cx;
                        bb += B_VAL(pix) * Cx;
                        aa += A_VAL(pix) * Cx;
                    }
                    if(j > 0){
                        pix++;
                        rr += R_VAL(pix) * j;
                        gg += G_VAL(pix) * j;
                        bb += B_VAL(pix) * j;
                        aa += A_VAL(pix) * j;
                    }
                    r = r * INV_YAP;
                    g = g * INV_YAP;
                    b = b * INV_YAP;
                    a = a * INV_YAP;
                    r = (r + ((rr * YAP))) >> 12;
                    g = (g + ((gg * YAP))) >> 12;
                    b = (b + ((bb * YAP))) >> 12;
                    a = (a + ((aa * YAP))) >> 12;
                }
                else{
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                    a >>= 4;
                }
                *dptr = qRgba(r >> 10, g >> 10, b >> 10, a >> 10);
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally & vertically */
    else{
        /*\ 'Correct' version, with math units prepared for MMXification:
         |*|  The operation 'b = (b * c) >> 16' translates to pmulhw,
         |*|  so the operation 'b = (b * c) >> d' would translate to
         |*|  psllw (16 - d), %mmb; pmulh %mmc, %mmb
         \*/
        int Cx, Cy, i, j;
        unsigned int *pix;
        int a, r, g, b, ax, rx, gx, bx;
        int xap, yap;

        for(y = 0; y < dh; y++){
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                sptr = ypoints[dyy + y] + xpoints[x];
                pix = sptr;
                sptr += sow;
                rx = R_VAL(pix) * xap;
                gx = G_VAL(pix) * xap;
                bx = B_VAL(pix) * xap;
                ax = A_VAL(pix) * xap;

                pix++;
                for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                    rx += R_VAL(pix) * Cx;
                    gx += G_VAL(pix) * Cx;
                    bx += B_VAL(pix) * Cx;
                    ax += A_VAL(pix) * Cx;
                    pix++;
                }
                if(i > 0){
                    rx += R_VAL(pix) * i;
                    gx += G_VAL(pix) * i;
                    bx += B_VAL(pix) * i;
                    ax += A_VAL(pix) * i;
                }

                r = (rx >> 5) * yap;
                g = (gx >> 5) * yap;
                b = (bx >> 5) * yap;
                a = (ax >> 5) * yap;

                for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                    pix = sptr;
                    sptr += sow;
                    rx = R_VAL(pix) * xap;
                    gx = G_VAL(pix) * xap;
                    bx = B_VAL(pix) * xap;
                    ax = A_VAL(pix) * xap;
                    pix++;
                    for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                        rx += R_VAL(pix) * Cx;
                        gx += G_VAL(pix) * Cx;
                        bx += B_VAL(pix) * Cx;
                        ax += A_VAL(pix) * Cx;
                        pix++;
                    }
                    if(i > 0){
                        rx += R_VAL(pix) * i;
                        gx += G_VAL(pix) * i;
                        bx += B_VAL(pix) * i;
                        ax += A_VAL(pix) * i;
                    }

                    r += (rx >> 5) * Cy;
                    g += (gx >> 5) * Cy;
                    b += (bx >> 5) * Cy;
                    a += (ax >> 5) * Cy;
                }
                if(j > 0){
                    pix = sptr;
                    sptr += sow;
                    rx = R_VAL(pix) * xap;
                    gx = G_VAL(pix) * xap;
                    bx = B_VAL(pix) * xap;
                    ax = A_VAL(pix) * xap;
                    pix++;
                    for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                        rx += R_VAL(pix) * Cx;
                        gx += G_VAL(pix) * Cx;
                        bx += B_VAL(pix) * Cx;
                        ax += A_VAL(pix) * Cx;
                        pix++;
                    }
                    if(i > 0){
                        rx += R_VAL(pix) * i;
                        gx += G_VAL(pix) * i;
                        bx += B_VAL(pix) * i;
                        ax += A_VAL(pix) * i;
                    }

                    r += (rx >> 5) * j;
                    g += (gx >> 5) * j;
                    b += (bx >> 5) * j;
                    a += (ax >> 5) * j;
                }

                *dptr = qRgba(r >> 23, g >> 23, b >> 23, a >> 23);
                dptr++;
            }
        }
    }
}

/* scale by area sampling - IGNORE the ALPHA byte*/
static void qt_qimageScaleAARGB(QImageScaleInfo *isi, unsigned int *dest,
                                int dxx, int dyy, int dx, int dy, int dw,
                                int dh, int dow, int sow)
{
    unsigned int *sptr, *dptr;
    int x, y, end;
    unsigned int **ypoints = isi->ypoints;
    int *xpoints = isi->xpoints;
    int *xapoints = isi->xapoints;
    int *yapoints = isi->yapoints;

    end = dxx + dw;
    /* scaling up both ways */
    if(isi->xup_yup == 3){
        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            /* calculate the source line we'll scan from */
            dptr = dest + dx + ((y + dy) * dow);
            sptr = ypoints[dyy + y];
            if(YAP > 0){
                for(x = dxx; x < end; x++){
                    int r = 0, g = 0, b = 0;
                    int rr = 0, gg = 0, bb = 0;
                    unsigned int *pix;

                    if(XAP > 0){
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_XAP;
                        g = G_VAL(pix) * INV_XAP;
                        b = B_VAL(pix) * INV_XAP;
                        pix++;
                        r += R_VAL(pix) * XAP;
                        g += G_VAL(pix) * XAP;
                        b += B_VAL(pix) * XAP;
                        pix += sow;
                        rr = R_VAL(pix) * XAP;
                        gg = G_VAL(pix) * XAP;
                        bb = B_VAL(pix) * XAP;
                        pix --;
                        rr += R_VAL(pix) * INV_XAP;
                        gg += G_VAL(pix) * INV_XAP;
                        bb += B_VAL(pix) * INV_XAP;
                        r = ((rr * YAP) + (r * INV_YAP)) >> 16;
                        g = ((gg * YAP) + (g * INV_YAP)) >> 16;
                        b = ((bb * YAP) + (b * INV_YAP)) >> 16;
                        *dptr++ = qRgba(r, g, b, 0xff);
                    }
                    else{
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_YAP;
                        g = G_VAL(pix) * INV_YAP;
                        b = B_VAL(pix) * INV_YAP;
                        pix += sow;
                        r += R_VAL(pix) * YAP;
                        g += G_VAL(pix) * YAP;
                        b += B_VAL(pix) * YAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        *dptr++ = qRgba(r, g, b, 0xff);
                    }
                }
            }
            else{
                for(x = dxx; x < end; x++){
                    int r = 0, g = 0, b = 0;
                    unsigned int *pix;

                    if(XAP > 0){
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = R_VAL(pix) * INV_XAP;
                        g = G_VAL(pix) * INV_XAP;
                        b = B_VAL(pix) * INV_XAP;
                        pix++;
                        r += R_VAL(pix) * XAP;
                        g += G_VAL(pix) * XAP;
                        b += B_VAL(pix) * XAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        *dptr++ = qRgba(r, g, b, 0xff);
                    }
                    else
                        *dptr++ = sptr[xpoints[x] ];
                }
            }
        }
    }
    /* if we're scaling down vertically */
    else if(isi->xup_yup == 1){
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cy, j;
        unsigned int *pix;
        int r, g, b, rr, gg, bb;
        int yap;

        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                pix = ypoints[dyy + y] + xpoints[x];
                r = R_VAL(pix) * yap;
                g = G_VAL(pix) * yap;
                b = B_VAL(pix) * yap;
                pix += sow;
                for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                    r += R_VAL(pix) * Cy;
                    g += G_VAL(pix) * Cy;
                    b += B_VAL(pix) * Cy;
                    pix += sow;
                }
                if(j > 0){
                    r += R_VAL(pix) * j;
                    g += G_VAL(pix) * j;
                    b += B_VAL(pix) * j;
                }
                if(XAP > 0){
                    pix = ypoints[dyy + y] + xpoints[x] + 1;
                    rr = R_VAL(pix) * yap;
                    gg = G_VAL(pix) * yap;
                    bb = B_VAL(pix) * yap;
                    pix += sow;
                    for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                        rr += R_VAL(pix) * Cy;
                        gg += G_VAL(pix) * Cy;
                        bb += B_VAL(pix) * Cy;
                        pix += sow;
                    }
                    if(j > 0){
                        rr += R_VAL(pix) * j;
                        gg += G_VAL(pix) * j;
                        bb += B_VAL(pix) * j;
                    }
                    r = r * INV_XAP;
                    g = g * INV_XAP;
                    b = b * INV_XAP;
                    r = (r + ((rr * XAP))) >> 12;
                    g = (g + ((gg * XAP))) >> 12;
                    b = (b + ((bb * XAP))) >> 12;
                }
                else{
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                }
                *dptr = qRgba(r >> 10, g >> 10, b >> 10, 0xff);
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally */
    else if(isi->xup_yup == 2){
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, j;
        unsigned int *pix;
        int r, g, b, rr, gg, bb;
        int xap;

        /* go through every scanline in the output buffer */
        for(y = 0; y < dh; y++){
            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                pix = ypoints[dyy + y] + xpoints[x];
                r = R_VAL(pix) * xap;
                g = G_VAL(pix) * xap;
                b = B_VAL(pix) * xap;
                pix++;
                for(j = (1 << 14) - xap; j > Cx; j -= Cx){
                    r += R_VAL(pix) * Cx;
                    g += G_VAL(pix) * Cx;
                    b += B_VAL(pix) * Cx;
                    pix++;
                }
                if(j > 0){
                    r += R_VAL(pix) * j;
                    g += G_VAL(pix) * j;
                    b += B_VAL(pix) * j;
                }
                if(YAP > 0){
                    pix = ypoints[dyy + y] + xpoints[x] + sow;
                    rr = R_VAL(pix) * xap;
                    gg = G_VAL(pix) * xap;
                    bb = B_VAL(pix) * xap;
                    pix++;
                    for(j = (1 << 14) - xap; j > Cx; j -= Cx){
                        rr += R_VAL(pix) * Cx;
                        gg += G_VAL(pix) * Cx;
                        bb += B_VAL(pix) * Cx;
                        pix++;
                    }
                    if(j > 0){
                        rr += R_VAL(pix) * j;
                        gg += G_VAL(pix) * j;
                        bb += B_VAL(pix) * j;
                    }
                    r = r * INV_YAP;
                    g = g * INV_YAP;
                    b = b * INV_YAP;
                    r = (r + ((rr * YAP))) >> 12;
                    g = (g + ((gg * YAP))) >> 12;
                    b = (b + ((bb * YAP))) >> 12;
                }
                else{
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                }
                *dptr = qRgba(r >> 10, g >> 10, b >> 10, 0xff);
                dptr++;
            }
        }
    }
    /* fully optimized (i think) - onyl change of algorithm can help */
    /* if we're scaling down horizontally & vertically */
    else{
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, Cy, i, j;
        unsigned int *pix;
        int r, g, b, rx, gx, bx;
        int xap, yap;

        for(y = 0; y < dh; y++){
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for(x = dxx; x < end; x++){
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                sptr = ypoints[dyy + y] + xpoints[x];
                pix = sptr;
                sptr += sow;
                rx = R_VAL(pix) * xap;
                gx = G_VAL(pix) * xap;
                bx = B_VAL(pix) * xap;
                pix++;
                for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                    rx += R_VAL(pix) * Cx;
                    gx += G_VAL(pix) * Cx;
                    bx += B_VAL(pix) * Cx;
                    pix++;
                }
                if(i > 0){
                    rx += R_VAL(pix) * i;
                    gx += G_VAL(pix) * i;
                    bx += B_VAL(pix) * i;
                }

                r = (rx >> 5) * yap;
                g = (gx >> 5) * yap;
                b = (bx >> 5) * yap;

                for(j = (1 << 14) - yap; j > Cy; j -= Cy){
                    pix = sptr;
                    sptr += sow;
                    rx = R_VAL(pix) * xap;
                    gx = G_VAL(pix) * xap;
                    bx = B_VAL(pix) * xap;
                    pix++;
                    for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                        rx += R_VAL(pix) * Cx;
                        gx += G_VAL(pix) * Cx;
                        bx += B_VAL(pix) * Cx;
                        pix++;
                    }
                    if(i > 0){
                        rx += R_VAL(pix) * i;
                        gx += G_VAL(pix) * i;
                        bx += B_VAL(pix) * i;
                    }

                    r += (rx >> 5) * Cy;
                    g += (gx >> 5) * Cy;
                    b += (bx >> 5) * Cy;
                }
                if(j > 0){
                    pix = sptr;
                    sptr += sow;
                    rx = R_VAL(pix) * xap;
                    gx = G_VAL(pix) * xap;
                    bx = B_VAL(pix) * xap;
                    pix++;
                    for(i = (1 << 14) - xap; i > Cx; i -= Cx){
                        rx += R_VAL(pix) * Cx;
                        gx += G_VAL(pix) * Cx;
                        bx += B_VAL(pix) * Cx;
                        pix++;
                    }
                    if(i > 0){
                        rx += R_VAL(pix) * i;
                        gx += G_VAL(pix) * i;
                        bx += B_VAL(pix) * i;
                    }

                    r += (rx >> 5) * j;
                    g += (gx >> 5) * j;
                    b += (bx >> 5) * j;
                }

                *dptr = qRgb(r >> 23, g >> 23, b >> 23);
                dptr++;
            }
        }
    }
}


static void qimageFreeScaleInfo(QImageScaleInfo *isi)
{
    if(isi)
    {
        delete[] isi->xpoints;
        delete[] isi->ypoints;
        delete[] isi->xapoints;
        delete[] isi->yapoints;
        delete isi;
    }
}


static unsigned char * createOutput(unsigned int width, unsigned int height)
{
    unsigned int depth = 32;
    const unsigned int bytesPerLine = ((width * depth + 31) >> 5) << 2; // bytes per scanline (must be multiple of 4)
    unsigned char * data = new unsigned char[bytesPerLine * height];
    return data;
}


unsigned char * smoothScaleImage(unsigned char * input, int alpha, int sw, int sh, int dw, int dh)
{
    int pixelsPerLine = sw; //src.bytesPerLine() >> 2;
    unsigned char * output = createOutput(dw, dh);

    QImageScaleInfo * scaleinfo = new QImageScaleInfo;
    memset(scaleinfo, 0, sizeof(QImageScaleInfo));

    scaleinfo->xup_yup = (qAbs(dw) >= sw) + ((qAbs(dh) >= sh) << 1);

    scaleinfo->xpoints = qimageCalcXPoints(sw, dw);
    scaleinfo->ypoints = qimageCalcYPoints((unsigned int *)input, pixelsPerLine, sh, dh);

    scaleinfo->xapoints = qimageCalcApoints(sw, dw, scaleinfo->xup_yup & 1);
    scaleinfo->yapoints = qimageCalcApoints(sh, dh, scaleinfo->xup_yup & 2);

    if (alpha)
        qt_qimageScaleAARGBA(scaleinfo, (unsigned int *)output, 0, 0, 0, 0, dw, dh, dw, pixelsPerLine);
    else
        qt_qimageScaleAARGB(scaleinfo, (unsigned int *)output, 0, 0, 0, 0, dw, dh, dw, pixelsPerLine);

    qimageFreeScaleInfo(scaleinfo);

    return output;
}

