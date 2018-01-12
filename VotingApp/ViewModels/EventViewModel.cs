using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Data.Entity;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventViewModel: PropertyChangedBase, IDisposable
    {
        private readonly VotingDbContext _votingDbContext;
        private readonly IWindowManager _windowManager;
        private Voting _selectedVoting;
        private Voting _votingInProgress;
        
        public EventViewModel(IWindowManager windowManager, Event @event)
        {
            Event = @event;
            _windowManager = windowManager;
            _votingDbContext = IoC.Get<VotingDbContext>();
            _votingDbContext.Voting.Where(v => v.IdEvent == @event.Id).Load();
        }

        public Event Event { get; }

        public IEnumerable<Voting> Votings => _votingDbContext.Voting.Local;

        public Voting SelectedVoting {
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
                _votingDbContext.Voting.Add(voting);
                _votingDbContext.SaveChanges();
                SelectedVoting = voting;
            }
        }

        public void StartVoting()
        {
            _selectedVoting.Start = DateTime.Now;
            _votingDbContext.SaveChanges();
            VotingInProgress = _selectedVoting;
        }

        public bool CanStartVoting => _votingInProgress == null && _selectedVoting != null && _selectedVoting.Start == null;

        public void StopVoting()
        {
            _votingInProgress.End = DateTime.Now;
            _votingDbContext.SaveChanges();
            VotingInProgress = null;
        }

        public bool CanStopVoting => _votingInProgress != null;

        public Voting VotingInProgress
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

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _votingDbContext.Dispose();
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