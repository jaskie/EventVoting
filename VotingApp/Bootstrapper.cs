using Caliburn.Micro;
using EventVoting.VotingApp.ViewModels;
using System;
using System.Collections.Generic;
using System.Windows;

namespace EventVoting.VotingApp
{
    public class Bootstrapper : BootstrapperBase
    {
        private SimpleContainer _container = new SimpleContainer();

        public Bootstrapper()
        {
            Initialize();
        }

        protected override void OnStartup(object sender, StartupEventArgs e)
        {
            _container
                .PerRequest<VotingDbContext>()
                .Singleton<IWindowManager, WindowManager>()
                .Singleton<MainViewModel>();
            DisplayRootViewFor<MainViewModel>();
        }

        protected override void BuildUp(object instance)
        {
            _container.BuildUp(instance);
        }

        protected override object GetInstance(Type service, string key)
        {
            return _container.GetInstance(service, key);
        }

        protected override IEnumerable<object> GetAllInstances(Type service)
        {
            return _container.GetAllInstances(service);
        }
    }
}
