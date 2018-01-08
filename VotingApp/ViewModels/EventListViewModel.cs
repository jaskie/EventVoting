using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventListViewModel : Screen
    {
        private Event _selectedEvent;
        private VotingDbContext _votingDbContext;

        public EventListViewModel()
        {
            _votingDbContext = IoC.Get<VotingDbContext>();
            _votingDbContext.Event.Load();
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

        public IEnumerable<Event> Events => _votingDbContext.Event.Local;
        

        public bool CanOk => SelectedEvent != null;

        public void Ok() => TryClose(true);

        public void Cancel() => TryClose();

        public void EditEvent(Event @event)
        {
            if (IoC.Get<IWindowManager>().ShowDialog(new EventPropertiesViewModel(@event)) == true)
            {
                _votingDbContext.SaveChanges();
            }
        }
    }
}
