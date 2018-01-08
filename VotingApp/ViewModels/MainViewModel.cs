using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace EventVoting.VotingApp.ViewModels
{
    public class MainViewModel : Screen
    {
        private EventViewModel _selectedEvent;
        private readonly IWindowManager _windowManager;

        public MainViewModel(IWindowManager windowManager)
        {
            _windowManager = windowManager;
            using (var context = IoC.Get<VotingDbContext>())
            {
                context.Appliance.Add(new Appliance { DeviceId = Guid.NewGuid().ToByteArray() });
                context.SaveChanges();
                context.Dispose();
            }
        }

        public void EventCreate()
        {
            var @event = new Event { Name = "New event" };
            if (_windowManager.ShowDialog(new EventPropertiesViewModel(@event)) == true)
            {
                using (var context = IoC.Get<VotingDbContext>())
                {
                    context.Event.Add(@event);
                    context.SaveChanges();
                }
                SelectedEvent = new EventViewModel(@event);
            }
        }

        public void EventSelect()
        {
            var eventList = new EventListViewModel();
            if (_windowManager.ShowDialog(eventList) == true)
                SelectedEvent = new EventViewModel(eventList.SelectedEvent);
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
