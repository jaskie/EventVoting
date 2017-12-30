using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.DomainModel
{
    public class Voting
    {
        [Key]
        public Int64 Id { get; set; }

        [MaxLength(100)]
        public String Name { get; set; }

        public DateTime? Start { get; set; }

        public DateTime? End { get; set; }

    }
}
