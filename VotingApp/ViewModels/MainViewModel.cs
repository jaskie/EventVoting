using Caliburn.Micro;

namespace EventVoting.VotingApp.ViewModels
{
    public class MainViewModel : Screen
    {
        private EventViewModel _selectedEvent;
        private PropertyChangedBase _selectedScreen;
        private readonly IWindowManager _windowManager;
        private static readonly string WindowTitle = "Voting controller";

        public MainViewModel(IWindowManager windowManager)
        {
            _windowManager = windowManager;
            DisplayName = WindowTitle;
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
            using (var eventList = new EventListViewModel())
            {
                if (_windowManager.ShowDialog(eventList) == true)
                    SelectedEvent = new EventViewModel(eventList.SelectedEvent);
            }
        }

        public void EventClose()
        {
            SelectedEvent = null;
        }

        public void EventShow()
        {
            SelectedScreen = SelectedEvent;
        }

        public void DeviceList()
        {
            SelectedScreen = new DeviceListViewModel();
        }

        public bool CanEventCreate => SelectedEvent == null;

        public bool CanEventSelect => SelectedEvent == null;

        public bool CanEventClose => SelectedEvent != null;

        public bool CanEventShow => SelectedEvent != null && SelectedScreen != SelectedEvent;

        public bool CanDeviceList => !(SelectedScreen is DeviceListViewModel);


        public PropertyChangedBase SelectedScreen
        {
            get => _selectedScreen;
            set
            {
                if (_selectedScreen == value)
                    return;
                (_selectedScreen as DeviceListViewModel)?.Dispose();
                _selectedScreen = value;
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanEventShow));
                NotifyOfPropertyChange(nameof(CanDeviceList));
            }
        }

        public EventViewModel SelectedEvent
        {
            get => _selectedEvent;
            set
            {
                if (_selectedEvent == value)
                    return;
                _selectedEvent?.Dispose();
                _selectedEvent = value;
                DisplayName = value == null ? WindowTitle : $"{WindowTitle} [{value.Event.Name}]";
                NotifyOfPropertyChange();
                NotifyOfPropertyChange(nameof(CanEventClose));
                NotifyOfPropertyChange(nameof(CanEventCreate));
                NotifyOfPropertyChange(nameof(CanEventSelect));
                if (value != null || SelectedScreen is EventViewModel)
                    SelectedScreen = value;
            }
        }

    }
}
