using Caliburn.Micro;
using System.Data.Entity;
using System;
using System.Collections.Generic;
using System.Linq;
using EventVoting.VotingApp.Model;

namespace EventVoting.VotingApp.ViewModels
{
    public class DeviceListViewModel: PropertyChangedBase, IDisposable
    {
        private readonly VotingDbContext _db;
        private readonly IWindowManager _windowManager;
        private DeviceViewModel _selectedDevice;

        public DeviceListViewModel()
        {
            _db = IoC.Get<VotingDbContext>();
            _windowManager = IoC.Get<IWindowManager>();
            _db.Device.Load();
            Devices = new BindableCollection<DeviceViewModel>(_db.Device.Local.Select(d => new DeviceViewModel(d)));
        }

        public void NewDevice()
        {
            var device = new Device { DeviceId = Guid.NewGuid().ToByteArray(), Type = (int)DeviceTypeEnum.LoRaRemote };
            _db.Device.Add(device);
            _db.SaveChanges();
            var vm = new DeviceViewModel(device);
            Devices.Add(vm);
            SelectedDevice = vm;
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


        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _db.Dispose();
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
