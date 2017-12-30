using Caliburn.Micro;
using EventVoting.VotingApp.ViewModels;
using System;
using System.Windows;

namespace EventVoting.VotingApp
{
    public class Bootstrapper : BootstrapperBase
    {
        public Bootstrapper()
        {
            Initialize();
        }

        protected override void OnStartup(object sender, StartupEventArgs e)
        {
            DisplayRootViewFor<MainViewModel>();
        }
    }
}
