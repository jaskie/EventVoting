//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace EventVoting.VotingApp
{
    using System;
    using System.Collections.Generic;
    
    public partial class Voting
    {
        public long Id { get; private set; }
        public long IdEvent { get; set; }
        public Nullable<System.DateTime> Start { get; set; }
        public Nullable<System.DateTime> End { get; set; }
        public string Name { get; set; }
    }
}
