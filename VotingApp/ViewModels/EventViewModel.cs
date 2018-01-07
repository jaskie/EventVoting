using Caliburn.Micro;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventViewModel: PropertyChangedBase
    {
        public EventViewModel(Event @event)
        {
            Event = @event;
        }

        public Event Event { get; }
    }
}