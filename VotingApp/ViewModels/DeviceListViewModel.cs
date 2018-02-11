using Caliburn.Micro;
using System.Data.Entity;
using System;
using System.Collections.Generic;
using System.Linq;
using EventVoting.VotingApp.Model;
using EventVoting.VotingApp.Hardware;

namespace EventVoting.VotingApp.ViewModels
{
    public class DeviceListViewModel: PropertyChangedBase, IDisposable
    {
        private readonly VotingDbContext _db;
        private readonly IWindowManager _windowManager;
        private DeviceViewModel _selectedDevice;
        private LoRaTransceiver _loRaTransceiver;

        public DeviceListViewModel()
        {
            _db = IoC.Get<VotingDbContext>();
            _windowManager = IoC.Get<IWindowManager>();
            _loRaTransceiver = IoC.Get<LoRaTransceiver>();
            _loRaTransceiver.DeviceResponse += _loRaTransceiver_DeviceResponse;
            _db.Device.Load();
            Devices = new BindableCollection<DeviceViewModel>(_db.Device.Local.Select(d => new DeviceViewModel(d)));
        }


        public void NewDevice()
        {
            _loRaTransceiver.RegisteredDevicesQuery();
        }

        public void DeleteDevice()
        {
            var vm = SelectedDevice;
            if (Devices.Remove(vm))
            {
                _db.Device.Remove(vm.Device);
                _db.SaveChanges();
            }
        }

        public bool CanDeleteDevice => SelectedDevice != null;

        public IList<DeviceViewModel> Devices { get; } 

        public DeviceViewModel SelectedDevice
        {
            get => _selectedDevice;
            set
            {
                if (_selectedDevice == value)
                    return;
                _selectedDevice = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanDeleteDevice));
            }
        }

        private void _loRaTransceiver_DeviceResponse(object sender, LoRaDeviceEventArgs e)
        {
            if (!(e.MessageType == LoRaMessageType.RegisteredDevicesQuery || e.MessageType == LoRaMessageType.RegisterNewDevice))
                return;
            if (Devices.Any(dvm =>  dvm.Device.DeviceId.SequenceEqual(e.DeviceId)))
                return;
            var device = new Device { DeviceId = e.DeviceId, Type = (int)DeviceTypeEnum.LoRaRemote };
            _db.Device.Add(device);
            _db.SaveChanges();
            var vm = new DeviceViewModel(device);
            Devices.Add(vm);
            SelectedDevice = vm;
        }
        

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _db.Dispose();
                    _loRaTransceiver.DeviceResponse -= _loRaTransceiver_DeviceResponse;
                }
                disposedValue = true;
            }
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
        }
        #endregion
    }
}
