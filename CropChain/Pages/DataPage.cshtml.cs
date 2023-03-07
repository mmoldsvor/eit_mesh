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
        private int nPlots;


        private readonly CropChain.Data.CropChainContext _context;

        public DataPageModel(CropChain.Data.CropChainContext context)
        {
            dataCollections = new List<DataCollection>();
            _context = context;
            nPlots = 0;
        }

        public void OnGet()
        {
            this.dataCollection = Deserializer.Get_Typed_DataCollection(_context, 5, "humid");
            this.dataCollections = Deserializer.Get_DataCollections_By_Sensor_ID(_context, 5);
            
            int graph_id = 0;
            foreach(DataCollection dataCollection in dataCollections) 
            {
                Console.WriteLine(dataCollection);
                ConstructChart(dataCollection, graph_id);
                graph_id++;
            }
            this.nPlots = dataCollections.Count();
        }

        private void ConstructChart(DataCollection dataCollection, int graph_id)
        {
            Plot dataPlot = new Plot();

            double[] y = new double[dataCollection.getAll().Count()];
            DateTime[] dateTimes = new DateTime[y.Length];
            
            for (int i = 0;i < y.Length; i++) 
            {
                y[i] = (double)dataCollection.getAll()[i].Data;
                dateTimes[i] = dataCollection.getAll()[i].Timestamp;
            }

            double[] x = dateTimes.Select(x => x.ToOADate()).ToArray();

            dataPlot.AddScatter(x, y);
            dataPlot.AddHorizontalLine((double)dataCollection.getLimit());
            dataPlot.XAxis.DateTimeFormat(true);
            dataPlot.XAxis2.Label("Sensor: " + dataCollection.getSensor_Id().ToString() + "  |  Data Type: " + dataCollection.getData_Type());
            dataPlot.SaveFig("wwwroot/resources/temp" + graph_id.ToString() + ".png");
        }

        public DataCollection GetDataCollection()
        {
            return this.dataCollection;
        }

        public int GetNPlots()
        {
            return this.nPlots;
        }
    }
}
