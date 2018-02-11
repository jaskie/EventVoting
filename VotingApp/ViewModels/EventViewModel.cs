using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Data.Entity;
using EventVoting.VotingApp.Hardware;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventViewModel: PropertyChangedBase, IDisposable
    {
        private readonly VotingDbContext _db;
        private readonly IWindowManager _windowManager;
        private VotingViewModel _selectedVoting;
        private VotingViewModel _votingInProgress;
        private readonly LoRaTransceiver _loRaTransceiver;
        
        public EventViewModel(Event @event)
        {
            Event = @event;
            _windowManager = IoC.Get<IWindowManager>();
            _db = IoC.Get<VotingDbContext>();
            _loRaTransceiver = IoC.Get<LoRaTransceiver>();
            _loRaTransceiver.VoteResponse += _loRaTransceiver_VoteResponse;
            _db.Voting.Where(v => v.IdEvent == @event.Id).Load();
            Votings = new BindableCollection<VotingViewModel>(_db.Voting.Local.Select(v => new VotingViewModel(v, _db, _loRaTransceiver)));
        }

        public Event Event { get; }

        public IList<VotingViewModel> Votings { get; }

        public VotingViewModel SelectedVoting {
            get => _selectedVoting;
            set
            {
                if (_selectedVoting == value)
                    return;
                _selectedVoting = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanStartVoting));
            }
        }

        public void NewVoting()
        {
            var voting = new Voting { Name = "New voting", IdEvent = Event.Id };
            if (_windowManager.ShowDialog(new VotingPropertiesViewModel(voting)) == true)
            {
                _db.Voting.Add(voting);
                _db.SaveChanges();
                var vm = new VotingViewModel(voting, _db, _loRaTransceiver);
                Votings.Add(vm);
                SelectedVoting = vm;
            }
        }

        public void StartVoting()
        {
            _selectedVoting.Start = DateTime.Now;
            _db.SaveChanges();
            VotingInProgress = _selectedVoting;
        }

        public bool CanStartVoting => _votingInProgress == null && _selectedVoting != null && _selectedVoting.Start == null;

        public void StopVoting()
        {
            _votingInProgress.End = DateTime.Now;
            var voting = _votingInProgress.Voting;
            _db.SaveChanges();
            VotingInProgress = null;
        }

        public bool CanStopVoting => _votingInProgress != null;

        public VotingViewModel VotingInProgress
        {
            get => _votingInProgress;
            set
            {
                if (_votingInProgress == value)
                    return;
                _votingInProgress = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanStopVoting));
                NotifyOfPropertyChange(nameof(CanStartVoting));
            }
        }

        private void _loRaTransceiver_VoteResponse(object sender, LoRaVoteResponseEventArgs e)
        {
            if (SelectedVoting == null)
                return;
            if (!SelectedVoting.CanFinishVoting)
                return;
            var device = _db.Device.FirstOrDefault(d => d.DeviceId == e.DeviceId);
            if (device == null)
                return;
            SelectedVoting.VoteResponse(device, e.Vote);
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
                    _loRaTransceiver.VoteResponse -= _loRaTransceiver_VoteResponse;
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