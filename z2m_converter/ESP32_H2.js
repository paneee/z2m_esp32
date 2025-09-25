import * as m from 'zigbee-herdsman-converters/lib/modernExtend';
import {
    Zcl
} from 'zigbee-herdsman';

// Helper function to create binary attributes
function createBinaryAttribute(name, cluster, attribute, valueOn, valueOff, description, access, entityCategory) {
    return m.binary({
        name,
        cluster,
        attribute,
        valueOn: valueOn || [true, 1],
        valueOff: valueOff || [false, 0],
        reporting: {
            min: '10_SECONDS',
            max: '1_HOUR',
            change: 1
        },
        description,
        scale: 1,
        access,
        entityCategory
    });
}

// Helper function to create numeric attributes
function createNumericAttribute(name, cluster, attribute, valueMin, valueMax, description, access, entityCategory) {
    return m.numeric({
        name,
        cluster,
        attribute,
        valueMin: valueMin || 0,
        valueMax: valueMax || 255,
        reporting: {
            min: '10_SECONDS',
            max: '1_HOUR',
            change: 1
        },
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
export default {
    zigbeeModel: ['ESP32_H2'],
    model: 'ESP32 H2',
    vendor: 'panee',
    description: 'DIY - Digital/Analog I/O',
    meta: {
        multiEndpoint: true
    },
    extend: [
        m.deviceEndpoints({
            "endpoints": {
                "basic": 1,
                "digital_input": 10,
                "digital_output": 20,
                "analog_input": 30,
                "analog_output": 40
            }
        }),
        m.deviceAddCustomCluster('digitalInput', {
            ID: 0xfd10,
            attributes: {
                valueCh1: {
                    ID: 0x0000,
                    type: Zcl.DataType.BOOLEAN
                },
                valueCh2: {
                    ID: 0x0001,
                    type: Zcl.DataType.BOOLEAN
                },
                valueCh3: {
                    ID: 0x0002,
                    type: Zcl.DataType.BOOLEAN
                },
            },
            commands: {},
            commandsResponse: {},
        }),
        digitalInput(1), digitalInput(2), digitalInput(3),

        m.deviceAddCustomCluster('digitalOutput', {
            ID: 0xfd11,
            attributes: {
                valueCh1: {
                    ID: 0x0000,
                    type: Zcl.DataType.BOOLEAN
                },
                valueCh2: {
                    ID: 0x0001,
                    type: Zcl.DataType.BOOLEAN
                },
                valueCh3: {
                    ID: 0x0002,
                    type: Zcl.DataType.BOOLEAN
                },
            },
            commands: {},
            commandsResponse: {},
        }),
        digitalOutput(1), digitalOutput(2), digitalOutput(3),

        m.deviceAddCustomCluster('analogInput', {
            ID: 0xfd12,
            attributes: {
                valueCh1: {
                    ID: 0x0000,
                    type: Zcl.DataType.UINT16
                },
                valueCh2: {
                    ID: 0x0001,
                    type: Zcl.DataType.UINT16
                },
                valueCh3: {
                    ID: 0x0002,
                    type: Zcl.DataType.UINT16
                },
            },
            commands: {},
            commandsResponse: {},
        }),
        analogInput(1), analogInput(2), analogInput(3),

        m.deviceAddCustomCluster('analogOutput', {
            ID: 0xfd13,
            attributes: {
                valueCh1: {
                    ID: 0x0000,
                    type: Zcl.DataType.UINT16
                },
                valueCh2: {
                    ID: 0x0001,
                    type: Zcl.DataType.UINT16
                },
                valueCh3: {
                    ID: 0x0002,
                    type: Zcl.DataType.UINT16
                },
            },
            commands: {},
            commandsResponse: {},
        }),
        analogOutput(1), analogOutput(2), analogOutput(3),
    ],
};