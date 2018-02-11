using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.Hardware
{
    public class LoRaVoteResponseEventArgs: EventArgs
    {
        public LoRaVoteResponseEventArgs(byte[] deviceId, byte vote)
        {
            DeviceId = deviceId;
            Vote = vote;
        }

        public byte[] DeviceId { get; }
        public byte Vote { get; }
    }
}
