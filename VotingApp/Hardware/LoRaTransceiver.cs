
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Concurrent;
using System.Diagnostics;
using WindowsSerialPort;

namespace EventVoting.VotingApp.Hardware
{
    public enum LoRaMessageType: byte
    {
        None,
        RegisteredDevicesQuery,
        RegisterNewDevice,
        DropDevice,
        StartVoting,
        EndVoting,
        DisplayMessage
    }

    public class LoRaTransceiver : IDisposable
    {

        const char NEWLINE = '\n';
        const int DEVICE_ID_LENGTH = 16;

        private Thread _serialWriteThread;
        private AutoResetEvent _writeThreadWaitEvent = new AutoResetEvent(false);

        private SerialPort _serial;
        private ConcurrentQueue<Action> _writeQueue = new ConcurrentQueue<Action>();
        private byte[] _readBuffer = new byte[0x100];
        private ushort _readBufferPos;


        public LoRaTransceiver(string comPort)
        {
            _serial = new SerialPort(comPort)
            {
                BaudRate = 115200,
                Handshake = Handshake.None,
                DataBits = 8,
                StopBits = StopBits.one,
                Parity = Parity.None,
                AutoReopen = true
            };
            _serial.Open();

            _serial.DataReceived += ByteReceived;
            _serialWriteThread = new Thread(CommWriteThreadProc)
            {
                IsBackground = true,
                Name = $"Serial port write thread for {comPort}"
            };
            _serialWriteThread.Start();
        }

        public void RegisteredDevicesQuery()
        {
            byte[] packet = BitConverter.GetBytes((byte)LoRaMessageType.RegisteredDevicesQuery);
            WriteThreadEnqueue(() => { WritePacket(packet); });
        }

        public void RegisterNewDevice(byte[] deviceId)
        {
            byte[] packet = BitConverter.GetBytes((byte)LoRaMessageType.RegisterNewDevice);
            int headerLength = packet.Length;
            Array.Resize(ref packet, headerLength + deviceId.Length);
            Buffer.BlockCopy(deviceId, 0, packet, headerLength, deviceId.Length);
            WriteThreadEnqueue(() =>  WritePacket(packet));
        }

        public void StartVoting(string message)
        {
            var messageBytes = Encoding.UTF8.GetBytes(message);
            var packet = new byte[messageBytes.Length + 1];
            packet[0] = (byte)LoRaMessageType.StartVoting;
            Buffer.BlockCopy(messageBytes, 0, packet, 1, messageBytes.Length);
            WriteThreadEnqueue(() => WritePacket(packet));
        }

        public event EventHandler<LoRaDeviceEventArgs> DeviceResponse;

        public event EventHandler<LoRaVoteResponseEventArgs> VoteResponse;

        #region privates

        private void WritePacket(byte[] packet)
        {
            var encoded = Encoding.ASCII.GetBytes(Convert.ToBase64String(packet)+NEWLINE);
            _serial.Write(encoded);
        }

        private void WriteThreadEnqueue(Action action)
        {
            _writeQueue.Enqueue(action);
            _writeThreadWaitEvent.Set();
        }

        private void CommWriteThreadProc()
        {
            var isAborted = false;
            while (!isAborted)
            {
                try
                {
                    if (_writeQueue.TryDequeue(out var action))
                        try
                        {
                            action();
                        }
                        catch (Exception e)
                        {
                            Debug.WriteLine(e);
                        }
                    else
                        _writeThreadWaitEvent.WaitOne();
                }
                catch(ThreadAbortException)
                {
                    isAborted = true;
                }
            }
        }
        
        private void ByteReceived(byte readByte)
        {
            if (readByte == NEWLINE)
            {
                if (_readBufferPos > 0)
                {
                    var result = Encoding.ASCII.GetString(_readBuffer, 0, _readBufferPos);
                    _readBufferPos = 0;
                    var response = Convert.FromBase64String(result);
                    ParseResponse(response);
                    Array.Clear(_readBuffer, 0, _readBuffer.Length);
                    return;
                }
            }
            if (_readBufferPos > _readBuffer.Length - 1)
                _readBufferPos = 0; // discard whole buffer - overflow
            _readBuffer[_readBufferPos++] = readByte;
        }

        private void ParseResponse(byte[] response)
        {
            if (response.Length == 0)
                return;
            Debug.WriteLine(Encoding.ASCII.GetString(response, 1, response.Length - 1));
            switch ((LoRaMessageType)response[0])
            {
                case LoRaMessageType.None:
                    break;
                case LoRaMessageType.RegisteredDevicesQuery:
                    if (response.Length >= DEVICE_ID_LENGTH + 1)
                    {
                        byte[] address = new byte[DEVICE_ID_LENGTH];
                        Buffer.BlockCopy(response, 1, address, 0, DEVICE_ID_LENGTH);
                        DeviceResponse?.Invoke(this, new LoRaDeviceEventArgs(LoRaMessageType.RegisteredDevicesQuery, address));
                    }
                    break;
                case LoRaMessageType.RegisterNewDevice:
                    if (response.Length >= DEVICE_ID_LENGTH + 1)
                    {
                        byte[] address = new byte[DEVICE_ID_LENGTH];
                        Buffer.BlockCopy(response, 1, address, 0, DEVICE_ID_LENGTH);
                        DeviceResponse?.Invoke(this, new LoRaDeviceEventArgs(LoRaMessageType.RegisterNewDevice, address));
                    }
                    break;
                case LoRaMessageType.StartVoting:
                    {
                        if (response.Length >= DEVICE_ID_LENGTH + 3)
                        {
                            byte[] address = new byte[DEVICE_ID_LENGTH];
                            Buffer.BlockCopy(response, 1, address, 0, DEVICE_ID_LENGTH);
                            VoteResponse?.Invoke(this, new LoRaVoteResponseEventArgs(address, response[DEVICE_ID_LENGTH + 2]));
                        }
                    }
                    break;

            }

        }
        #endregion private

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _serialWriteThread.Abort();
                    _serial.DataReceived -= ByteReceived;
                    _serial.Dispose();
                }
                disposedValue = true;
            }
        }

        // TODO: override a finalizer only if Dispose(bool disposing) above has code to free unmanaged resources.
        // ~LoRaTransceiver() {
        //   // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
        //   Dispose(false);
        // }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            // TODO: uncomment the following line if the finalizer is overridden above.
            // GC.SuppressFinalize(this);
        }
        #endregion

    }
}
