using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventPropertiesViewModel: Screen
    {
        public EventPropertiesViewModel(Event @event)
        {
            Event = @event;
        }

        public Event Event { get; }

        public void Ok() => TryClose(true);

        public void Cancel() => TryClose();

    }
}
