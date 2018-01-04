using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class MainViewModel : Screen
    {
        private EventViewModel _selectedEvent;

        public MainViewModel()
        {
            var context = ApplicationData.Current.DbContext;
            context.Appliance.Add(new Appliance { DeviceId = Guid.NewGuid().ToByteArray() });
            context.SaveChanges();
            context.Dispose();
        }

        public void EventCreate()
        {
            SelectedEvent = new EventViewModel();
        }

        public void EventSelect()
        {

        }

        public void EventClose()
        {
            SelectedEvent = null;
        }

        public bool CanEventCreate => SelectedEvent == null;

        public bool CanEventSelect => SelectedEvent == null;

        public bool CanEventClose => SelectedEvent != null;


        public EventViewModel SelectedEvent
        {
            get => _selectedEvent;
            set
            {
                if (_selectedEvent == value)
                    return;
                _selectedEvent = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanEventClose));
                NotifyOfPropertyChange(nameof(CanEventCreate));
                NotifyOfPropertyChange(nameof(CanEventSelect));
            }
        }

    }
}
