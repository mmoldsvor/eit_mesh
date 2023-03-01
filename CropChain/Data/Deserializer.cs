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

        internal static DataCollection ConstructData(List<Measurements> data)
        {
            List<Models.DataPoint> dataPoints = new List<Models.DataPoint>();
            
            foreach (var m in data) 
            {
                dataPoints.Add(new Models.DataPoint((float)m.Data,
                    m.Timestamp));
            }
            dataPoints.Reverse();
            return new DataCollection(dataPoints);
        }

    }
}
