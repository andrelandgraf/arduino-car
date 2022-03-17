import { useCallback, useEffect, useRef, useState } from 'react';
import { useStateRef } from '~/useStateRef';

declare global {
  interface Navigator {
    bluetooth: {
      requestDevice(options: {
        acceptAllDevices?: boolean;
        filters?: Array<
          | {
              services: string[] | number[];
            }
          | { name: string }
          | { namePrefix: string }
        >;
      }): Promise<BluetoothDevice>;
    };
  }
}

enum BluetoothSignals {
  'forward' = 'W',
  'backward' = 'S',
  'left' = 'A',
  'right' = 'D',
  'stop' = 'Q',
}

type BluetoothDevice = {
  id: string;
  name: string;
  gatt: { connect: () => Promise<Server>; connected: boolean };
  addEventListener: Window['addEventListener'];
  removeEventListener: Window['removeEventListener'];
};

type Server = {
  getPrimaryService: (service: string | number) => Promise<Service>;
};

type Service = {
  getCharacteristic: (characteristic: string | number) => Promise<Characteristic>;
};

type Characteristic = {
  startNotifications: () => Promise<void>;
  readValue: () => Promise<Uint8Array>;
  writeValue: (value: Uint8Array) => Promise<void>;
  addEventListener: Window['addEventListener'];
  removeEventListener: Window['removeEventListener'];
};

export default function Index() {
  const deviceRef = useRef<BluetoothDevice | null>(null);
  const characteristicRef = useRef<Characteristic | null>(null);
  const [bluetoothMsgs, setBluetoothMsgs] = useState<string[]>([]);
  const [, setBluetoothMsg, bluetoothMsgRef] = useStateRef('');
  const [errorMessage, setErrorMessage] = useState('');
  const [connectionState, setConnectionState] = useState<
    'idle' | 'device found' | 'server found' | 'service found' | 'characteristic found' | 'connected' | 'disconnected'
  >('idle');

  const handleDisconnect = useCallback(() => {
    console.log(`${deviceRef.current?.name} bluetooth device disconnected, trying to reconnect...`);
    setConnectionState('disconnected');
    if (characteristicRef.current) {
      startNotifications(characteristicRef.current);
    }
  }, []);

  const receiveData = useCallback((event: any) => {
    console.log('Received data...');
    const next = new TextDecoder().decode(event.target.value);
    for (const char of next) {
      if (char === '\n') {
        const currMsg = bluetoothMsgRef.current;
        setBluetoothMsgs((prev) => [...prev, currMsg]);
        setBluetoothMsg('');
      } else {
        setBluetoothMsg((v) => v + next);
      }
    }
  }, []);

  const sendData = useCallback((data: string) => {
    console.log('Sending data...');
    if (!data || !characteristicRef.current) {
      return;
    }
    characteristicRef.current.writeValue(new TextEncoder().encode(data + '\r\n'));
  }, []);

  const startNotifications = useCallback(async (characteristic: Characteristic) => {
    console.log('Starting notifications...');

    await characteristic.startNotifications();
    setConnectionState('connected');
    console.log('Notifications started!');
    characteristic.addEventListener('characteristicvaluechanged', receiveData);
  }, []);

  const connect = useCallback(async () => {
    console.log('Connecting...');
    try {
      if (deviceRef.current?.gatt.connected && characteristicRef.current) {
        startNotifications(characteristicRef.current);
        return;
      }
      const device = await navigator.bluetooth.requestDevice({ filters: [{ services: [0xffe0] }] });
      console.log(`Device found with id: ${device.id} and name: ${device.name}`);
      deviceRef.current = device;
      deviceRef.current.addEventListener('gattserverdisconnected', handleDisconnect);
      setConnectionState('device found');
      const server = await device.gatt.connect();
      setConnectionState('server found');
      const service = await server.getPrimaryService(0xffe0);
      setConnectionState('service found');
      const characteristic = await service.getCharacteristic(0xffe1);
      characteristicRef.current = characteristic;
      setConnectionState('characteristic found');
      startNotifications(characteristic);
    } catch (error: any) {
      console.error(error);
      setErrorMessage(error.message);
    }
  }, []);

  useEffect(() => {
    return () => {
      if (characteristicRef.current) {
        characteristicRef.current.removeEventListener('characteristicvaluechanged', receiveData);
      }
      if (deviceRef.current) {
        deviceRef.current.removeEventListener('gattserverdisconnected', handleDisconnect);
      }
    };
  }, []);

  return (
    <main className="w-full flex flex-col gap-10 items-center justify-center mt-10">
      <h1 className="text-xl xl:text-4xl">Arduino Car Control Center</h1>
      <p className="text-lg xl:text-xl text-green-600">{`Current connection state: ${connectionState}`}</p>
      {errorMessage && <p className="text-red-600">{errorMessage}</p>}
      <button
        onClick={connect}
        className="hover:bg-primary focus:bg-primary border-4 border-primary rounded-full focus:outline-offset-4 focus:outline-lime-300 py-2 px-4"
      >
        Connect
      </button>
      {connectionState === 'connected' && (
        <div className="w-full flex flex-col gap-2 p-5 border-1 border-black text-gray-400">
          <h2>Messages from Arduino:</h2>
          {bluetoothMsgs.map((msg) => (
            <p className="text-lg xl:text-xl" key={msg}>
              {msg}
            </p>
          ))}
        </div>
      )}
      {connectionState === 'connected' && (
        <section>
          <h2 className="sr-only">Car Controls</h2>
          <div className="flex flex-col gap-5 font-bold text-6xl">
            <button
              className="transform hover:scale-105"
              aria-label="forwards"
              onClick={() => sendData(BluetoothSignals.forward)}
            >
              ⬆️
            </button>
            <button
              className="transform hover:scale-105"
              aria-label="backwards"
              onClick={() => sendData(BluetoothSignals.stop)}
            >
              ⏹️
            </button>
            <button
              className="transform hover:scale-105"
              aria-label="backwards"
              onClick={() => sendData(BluetoothSignals.backward)}
            >
              ⬇️
            </button>
          </div>
        </section>
      )}
    </main>
  );
}
