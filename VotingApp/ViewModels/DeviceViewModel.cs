using System;

namespace EventVoting.VotingApp.ViewModels
{
    public class DeviceViewModel
    {
        public DeviceViewModel(Device device)
        {
            Device = device;
        }

        public Device Device { get; }

        public string DeviceId => BitConverter.ToString(Device.DeviceId);
    }
}