const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const ea = exposes.access;

const {
    Fz,
    Tz,
    ModernExtend,
    Expose,
} = 'zigbee-herdsman-converters/lib/types';

const { deviceAddCustomCluster, numeric, binary } = require('zigbee-herdsman-converters/lib/modernExtend');
const Zcl = require('zigbee-herdsman').Zcl;

// Helper function to create binary attributes
function createBinaryAttribute(name, cluster, attribute, valueOn, valueOff, description, access, entityCategory) {
    return binary({
        name,
        cluster,
        attribute,
        valueOn: valueOn || [true, 1],
        valueOff: valueOff || [false, 0],
        reporting: { min: '10_SECONDS', max: '1_HOUR', change: 1 },
        description,
        scale: 1,
        access,
        entityCategory
    });
}

// Helper function to create numeric attributes
function createNumericAttribute(name, cluster, attribute, valueMin, valueMax, description, access, entityCategory) {
    return numeric({
        name,
        cluster,
        attribute,
        valueMin: valueMin || 0,
        valueMax: valueMax || 255,
        reporting: { min: '10_SECONDS', max: '1_HOUR', change: 1 },
        description,
        scale: 1,
        access,
        entityCategory
    });
}

// Digital input function
function digitalInput(channel) {
    return createBinaryAttribute(
        `Digital Input Value ${channel}`, 
        'digitalInput', 
        `valueCh${channel}`, 
        [true, 1], 
        [false, 0], 
        'From ESP', 
        'STATE_GET', 
    );
}

// Digital output function
function digitalOutput(channel) {
    return createBinaryAttribute(
        `Digital Output Value ${channel}`, 
        'digitalOutput', 
        `valueCh${channel}`, 
        [true, 1], 
        [false, 0], 
        'To ESP', 
        'STATE_SET'
    );
}

// Analog input functions
function analogInput(channel) {
    return createNumericAttribute(
        `Analog Input Value ${channel}`, 
        'analogInput', 
        `valueCh${channel}`, 
        0, 
        255, 
        'From ESP', 
        'STATE_GET', 
    );
}

// Analog output functions
function analogOutput(channel) {
    return createNumericAttribute(
        `Analog Output Value ${channel}`, 
        'analogOutput', 
        `valueCh${channel}`, 
        0, 
        255, 
        'To ESP', 
        'STATE_SET', 
    );
}

// Zigbee device definition
const definition = {
    zigbeeModel: ['ESP32_H2'],
    model: 'ESP32 H2',
    vendor: 'panee',
    description: 'DIY - Digital/Analog I/O',
    endpoint: (device) => {
        return {
            basic: 1,
            digital_input: 10,
            digital_output: 20,
            analog_input: 30,
            analog_output: 40,
        };
    },
    extend: [
        deviceAddCustomCluster('digitalInput', {
            ID: 0xfd10,
            attributes: {
                valueCh1: { ID: 0x0000, type: Zcl.DataType.BOOLEAN },
                valueCh2: { ID: 0x0001, type: Zcl.DataType.BOOLEAN },
                valueCh3: { ID: 0x0002, type: Zcl.DataType.BOOLEAN },
            },
            commands: {},
            commandsResponse: {},
        }), 
        digitalInput(1), digitalInput(2), digitalInput(3),

        deviceAddCustomCluster('digitalOutput', {
            ID: 0xfd11,
            attributes: {
                valueCh1: { ID: 0x0000, type: Zcl.DataType.BOOLEAN },
                valueCh2: { ID: 0x0001, type: Zcl.DataType.BOOLEAN },
                valueCh3: { ID: 0x0002, type: Zcl.DataType.BOOLEAN },
            },
            commands: {},
            commandsResponse: {},
        }), 
        digitalOutput(1), digitalOutput(2), digitalOutput(3),

        deviceAddCustomCluster('analogInput', {
            ID: 0xfd12,
            attributes: {
                valueCh1: { ID: 0x0000, type: Zcl.DataType.UINT16 },
                valueCh2: { ID: 0x0001, type: Zcl.DataType.UINT16 },
                valueCh3: { ID: 0x0002, type: Zcl.DataType.UINT16 },
            },
            commands: {},
            commandsResponse: {},
        }), 
        analogInput(1), analogInput(2), analogInput(3),

        deviceAddCustomCluster('analogOutput', {
            ID: 0xfd13,
            attributes: {
                valueCh1: { ID: 0x0000, type: Zcl.DataType.UINT16 },
                valueCh2: { ID: 0x0001, type: Zcl.DataType.UINT16 },
                valueCh3: { ID: 0x0002, type: Zcl.DataType.UINT16 },
            },
            commands: {},
            commandsResponse: {},
        }), 
        analogOutput(1), analogOutput(2), analogOutput(3),
    ],
    meta: { multiEndpoint: true },
};

module.exports = definition;
