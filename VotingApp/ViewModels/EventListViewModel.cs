using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Data.Entity;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventListViewModel : Screen, IDisposable
    {
        private Event _selectedEvent;
        private readonly VotingDbContext _db;

        public EventListViewModel()
        {
            _db = IoC.Get<VotingDbContext>();
            _db.Event.Load();
        }

        public Event SelectedEvent
        {
            get => _selectedEvent;
            set
            {
                if (_selectedEvent == value)
                    return;
                _selectedEvent = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanOk));
            }
        }

        public IEnumerable<Event> Events => _db.Event.Local;


        public bool CanOk => SelectedEvent != null;

        public void Ok() => TryClose(true);

        public void Cancel() => TryClose();

        public void EditEvent(Event @event)
        {
            if (IoC.Get<IWindowManager>().ShowDialog(new EventPropertiesViewModel(@event)) == true)
            {
                _db.SaveChanges();
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
