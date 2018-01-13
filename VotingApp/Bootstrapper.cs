using Caliburn.Micro;
using EventVoting.VotingApp.Database;
using EventVoting.VotingApp.ViewModels;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using System.Windows.Markup;

namespace EventVoting.VotingApp
{
    public class Bootstrapper : BootstrapperBase
    {
        private SimpleContainer _container = new SimpleContainer();

        public Bootstrapper()
        {
            Initialize();
            if (!(Application.Current is App)) // in design mode
                return;
            FrameworkElement.LanguageProperty.OverrideMetadata(
              typeof(FrameworkElement),
              new FrameworkPropertyMetadata(
                  XmlLanguage.GetLanguage(Thread.CurrentThread.CurrentCulture.Name)));
        }

        protected override void OnStartup(object sender, StartupEventArgs e)
        {
            System.Data.Entity.Database.SetInitializer(new DatabaseInitializer());
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
