using CropChain.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using System;

namespace CropChain.Data
{
    public sealed class Deserializer
    {
        // Boilerplate begins
        private static Deserializer instance = null;
        private static readonly object padlock = new object();

        Deserializer()
        {
        }

        public static Deserializer Instance
        {
            get
            {
                lock (padlock)
                {
                    if (instance == null)
                    {
                        instance = new Deserializer();
                    }
                    return instance;
                }
            }
        }

        // Boilerplate ends

        public static void Initialize() {}


        public static DataCollection GetDataCollection(CropChain.Data.CropChainContext context, int n)
        {
            return ConstructData(context.Measurements.OrderByDescending(x => x.Timestamp).Take(n).ToList());
        }

        public static DataCollection Get_Typed_DataCollection(CropChain.Data.CropChainContext context, int n, string type)
        {
            return ConstructData(context.Measurements.OrderByDescending(x => x.Timestamp).Where(x => x.Data_Type == type).Take(n).ToList());
        }

        public static List<DataCollection> Get_DataCollections_By_Sensor_ID(CropChain.Data.CropChainContext context, int n)
        {
            List<int> ids = GetSensorIDs(context);
            List<DataCollection> collections = new List<DataCollection>();

            foreach (int id in ids)
            {
                collections.Add(
                    ConstructData(
                        context.Measurements.OrderByDescending(x => x.Timestamp).Where(x => x.Sensor_Id == id).Take(n).ToList()
                    ));
            }


            return collections;
        }
        

        internal static List<int> GetSensorIDs(CropChain.Data.CropChainContext context) 
        {
            List<Measurements> temp = context.Measurements.GroupBy(x => x.Sensor_Id).Select(g => g.OrderBy(x => x.Sensor_Id).First()).ToList();

            List<int> ids = new List<int>();
            foreach (Measurements measurements in temp) 
            { 
                ids.Add(measurements.Sensor_Id);
            }


            return ids;
        }

        internal static DataCollection ConstructData(List<Measurements> data)
        {
            List<Models.DataPoint> dataPoints = new List<Models.DataPoint>();
            
            foreach (var m in data) 
            {
                dataPoints.Add(new Models.DataPoint((float)m.Data, m.Timestamp));
            }
            dataPoints.Reverse();
            return new DataCollection(dataPoints, data.First().Data_Type, data.First().Sensor_Id);
        }

    }
}
