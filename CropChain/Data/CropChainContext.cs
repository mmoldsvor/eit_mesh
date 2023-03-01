using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;

namespace CropChain.Data
{
    public class CropChainContext : DbContext
    {
        public CropChainContext (DbContextOptions<CropChainContext> options)
            : base(options)
        {
        }

        public DbSet<CropChain.Data.Measurements> Measurements { get; set; } = default!;
    }
}
