using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.Hardware
{
    public class LoRaDeviceEventArgs: EventArgs
    {
        public LoRaDeviceEventArgs(LoRaMessageType messageType,  byte[] deviceId)
        {
            MessageType = messageType;
            DeviceId = deviceId;
        }

        public LoRaMessageType MessageType { get; }
        public byte[] DeviceId { get; }
    }
}
