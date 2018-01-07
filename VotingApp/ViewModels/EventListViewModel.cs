using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class EventListViewModel : Screen
    {
        public Event SelectedEvent { get; set; }

        public IEnumerable<Event> Events
        {
            get
            {
                using (var ctx = IoC.Get<VotingDbContext>())
                    return ctx.Event.Local;
            }
        }
    }
}
