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

        private readonly CropChain.Data.CropChainContext _context;
        private List<DataCollection> dataCollections;
        private int nPlots;

        public DataPageModel(CropChain.Data.CropChainContext context)
        {
            dataCollections = new List<DataCollection>();
            _context = context;
            nPlots = 0;
        }

        public void OnGet()
        {
            this.dataCollections = Deserializer.Get_DataCollections_By_Sensor_ID(_context, 5);
            
            int graph_id = 0;
            foreach(DataCollection dataCollection in dataCollections) 
            {
                ConstructChart(dataCollection, graph_id);
                graph_id++;
            }
            this.nPlots = dataCollections.Count;
        }

        private void ConstructChart(DataCollection dataCollection, int graph_id)
        {
            Plot dataPlot = new Plot();

            // Setup data
            double[] y = new double[dataCollection.GetAll().Count];
            DateTime[] dateTimes = new DateTime[y.Length];
            
            for (int i = 0;i < y.Length; i++) 
            {
                y[i] = (double)dataCollection.GetAll()[i].Data;
                dateTimes[i] = dataCollection.GetAll()[i].Timestamp;
            }

            double[] x = dateTimes.Select(x => x.ToOADate()).ToArray();

            
            // Configure plot
            dataPlot.AddScatter(x, y);
            
            dataPlot.AddHorizontalLine((double)dataCollection.GetLimit()[0]);
            dataPlot.AddHorizontalLine((double)dataCollection.GetLimit()[1]);
            
            dataPlot.XAxis.DateTimeFormat(true);
            
            dataPlot.XAxis2.Label("Sensor: " 
                + dataCollection.GetSensor_Id().ToString() 
                + "  |  Data Type: " 
                + dataCollection.GetData_Type());

            dataPlot.SaveFig("wwwroot/resources/temp" + graph_id.ToString() + ".png");
        }

        public int GetNPlots()
        {
            return this.nPlots;
        }
    }
}
