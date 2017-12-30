using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class MainViewModel: Screen
    {
        public MainViewModel()
        {
            var context = ApplicationData.Current.DbContext;
            context.Appliance.Add(new Appliance { DeviceId = Guid.NewGuid().ToByteArray() });
            context.SaveChanges();
            context.Dispose();
        }


    }
}
