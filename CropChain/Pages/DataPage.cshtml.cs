using CropChain.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using System.Reflection;
using ScottPlot;
using CropChain.Data;

namespace CropChain.Pages
{
    public class DataPageModel : PageModel
    {

        private DataCollection dataCollection;
        private List<DataCollection> dataCollections;
        private Plot dataPlot = new Plot();


        private readonly CropChain.Data.CropChainContext _context;

        public DataPageModel(CropChain.Data.CropChainContext context)
        {
            dataCollections = new List<DataCollection>();
            _context = context;
        }

        public void OnGet()
        {
            this.dataCollection = Deserializer.Get_Typed_DataCollection(_context, 5, "humid");
            ConstructChart();
        }

        private void ConstructChart()
        {
            double[] y = new double[this.dataCollection.getAll().Count()];
            DateTime[] dateTimes = new DateTime[y.Length];
            
            for (int i = 0;i < y.Length; i++) 
            {
                y[i] = (double)this.dataCollection.getAll()[i].Data;
                dateTimes[i] = this.dataCollection.getAll()[i].Timestamp;
            }

            double[] x = dateTimes.Select(x => x.ToOADate()).ToArray();

            this.dataPlot.AddScatter(x, y);
            this.dataPlot.AddHorizontalLine((double)this.dataCollection.getLimit());
            this.dataPlot.XAxis.DateTimeFormat(true);
            this.dataPlot.SaveFig("wwwroot/resources/temp1.png");
        }

        public DataCollection GetDataCollection()
        {
            return this.dataCollection;
        }

        public Plot GetPlot()
        {
            return this.dataPlot;
        }
    }
}
