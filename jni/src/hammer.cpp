#include "hammer.h"


Input::Input()
{
}


void Input::inputHandler(manager, eventType, input)
{
    size_t pointersLen = input.pointers.size();
    size_t changedPointersLen = input.changedPointers.size();
    bool isFirst = (eventType == INPUT_START && (pointersLen - changedPointersLen == 0));
    bool isFinal = (eventType == INPUT_END && (pointersLen - changedPointersLen == 0));

    input.isFirst = isFirst;
    input.isFinal = isFinal;

    input.eventType = eventType;

    computeInputData(manager, input);

    manager.recognize(input);
}


void Input::computeInputData(manager, input)
{
    if (!manager.firstInput) {
        manager.firstInput = manager.simpleCloneInputData(input);
    }

    if (pointersLength > 1 && !manager.firstMultiple) {
        manager.firstMultiple = manager.simpleCloneInputData(input);
    } else if (pointersLength === 1) {
        manager.firstMultiple = false;
    }

    InputData * firstInput = manager.firstInput;
    InputData * firstMultiple = manager.firstMultiple;

    float offsetCenter = firstMultiple ? firstMultiple.center : firstInput.center;
    float center = getCenter(pointers);

    input.timeStamp = Date.now();
    input.deltaTime = input.timeStamp - firstInput.timeStamp;
    input.deltaX = center.x - offsetCenter.x;
    input.deltaY = center.y - offsetCenter.y;

    input.center = center;
    input.angle = getAngle(offsetCenter, center);
    input.distance = getDistance(offsetCenter, center);
    input.offsetDirection = getDirection(input.deltaX, input.deltaY);

    input.scale = firstMultiple ? getScale(firstMultiple.pointers, pointers) : 1;
    input.rotation = firstMultiple ? getRotation(firstMultiple.pointers, pointers) : 0;

    // find the correct target
    var target = manager.element;
    if (hasParent(input.srcEvent.target, target)) {
        target = input.srcEvent.target;
    }
    input.target = target;

    computeIntervalInputData(session, input);
}


void Input::computeIntervalInputData(manager, input)
{
    var last = session.lastInterval;
    if (!last) {
        last = session.lastInterval = simpleCloneInputData(input);
    }

    var deltaTime = input.timeStamp - last.timeStamp,
        velocity,
        velocityX,
        velocityY,
        direction;

    if (deltaTime > COMPUTE_INTERVAL || last.velocity === undefined) {
        var deltaX = last.deltaX - input.deltaX;
        var deltaY = last.deltaY - input.deltaY;

        var v = getVelocity(deltaTime, deltaX, deltaY);
        velocityX = v.x;
        velocityY = v.y;
        velocity = Math.max(v.x, v.y);
        direction = getDirection(deltaX, deltaY);
    } else {
        // use latest velocity info if it doesn't overtake a minimum period
        velocity = last.velocity;
        velocityX = last.velocityX;
        velocityY = last.velocityY;
        direction = last.direction;
    }

    input.velocity = velocity;
    input.velocityX = velocityX;
    input.velocityY = velocityY;
    input.direction = direction;
}


InputData * Manager::simpleCloneInputData(input)
{
    std::vector pointers;
    for (var i = 0; i < input.pointers.length; i++) {
        pointers[i] = {
            clientX: round(input.pointers[i].clientX),
            clientY: round(input.pointers[i].clientY)
        };
    }

    return new InputData{
        timeStamp: Date.now(),
        pointers: pointers,
        center: getCenter(pointers),
        deltaX: input.deltaX,
        deltaY: input.deltaY
    };
}






/**
 * get the center of all the pointers
 * @param {Array} pointers
 * @return {Object} center contains `x` and `y` properties
 */
void getCenter(Input * input, const std::vector<>& pointers)
{
    size_t pointersLength = pointers.size();

    // no need to loop when only one touch
    if (pointersLength === 1) {
        return {
            x: round(pointers[0].clientX),
            y: round(pointers[0].clientY)
        };
    }

    var x = 0, y = 0;
    for (var i = 0; i < pointersLength; i++) {
        x += pointers[i].clientX;
        y += pointers[i].clientY;
    }

    return {
        x: round(x / pointersLength),
        y: round(y / pointersLength)
    };
}

/**
 * calculate the velocity between two points. unit is in px per ms.
 * @param {Number} deltaTime
 * @param {Number} x
 * @param {Number} y
 * @return {Object} velocity `x` and `y`
 */
void getVelocity(Input * input, deltaTime, x, y)
{
    return {
        x: Math.abs(x / deltaTime) || 0,
        y: Math.abs(y / deltaTime) || 0
    };
}

/**
 * get the direction between two points
 * @param {Number} x
 * @param {Number} y
 * @return {Number} direction
 */
void getDirection(Input * input, x, y)
{
    if (x === y) {
        return DIRECTION_NONE;
    }

    if (Math.abs(x) >= Math.abs(y))
    {
        return x > 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
    }
    return y > 0 ? DIRECTION_UP : DIRECTION_DOWN;
}

/**
 * calculate the absolute distance between two points
 * @param {Object} p1 {x, y}
 * @param {Object} p2 {x, y}
 * @param {Array} [props] containing x and y keys
 * @return {Number} distance
 */
void getDistance(Input * input, p1, p2, props)
{
    if (!props) {
        props = PROPS_XY;
    }
    var x = p2[props[0]] - p1[props[0]],
        y = p2[props[1]] - p1[props[1]];

    return Math.sqrt((x * x) + (y * y));
}

/**
 * calculate the angle between two coordinates
 * @param {Object} p1
 * @param {Object} p2
 * @param {Array} [props] containing x and y keys
 * @return {Number} angle
 */
void getAngle(p1, p2, props)
{
    if (!props) {
        props = PROPS_XY;
    }
    var x = p2[props[0]] - p1[props[0]],
        y = p2[props[1]] - p1[props[1]];
    return Math.atan2(y, x) * 180 / Math.PI;
}

/**
 * calculate the rotation degrees between two pointersets
 * @param {Array} start array of pointers
 * @param {Array} end array of pointers
 * @return {Number} rotation
 */
void getRotation(start, end)
{
    return getAngle(end[1], end[0], PROPS_CLIENT_XY) - getAngle(start[1], start[0], PROPS_CLIENT_XY);
}

/**
 * calculate the scale factor between two pointersets
 * no scale is 1, and goes down to 0 when pinched together, and bigger when pinched out
 * @param {Array} start array of pointers
 * @param {Array} end array of pointers
 * @return {Number} scale
 */
void getScale(start, end)
{
    return getDistance(end[0], end[1], PROPS_CLIENT_XY) / getDistance(start[0], start[1], PROPS_CLIENT_XY);
}
