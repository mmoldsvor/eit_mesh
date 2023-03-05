using Microsoft.EntityFrameworkCore;

namespace CropChain.Data;

public static class SeedData
{
    public static void Initialize(IServiceProvider serviceProvider)
    {
        using (var context = new CropChainContext(
            serviceProvider.GetRequiredService<
                DbContextOptions<CropChainContext>>()))
        {
            if (context == null || context.Measurements == null)
            {
                throw new ArgumentNullException("Null RazorPagesMovieContext");
            }

            // Look for any measurements.
            if (context.Measurements.Any())
            {
                return;   // DB has been seeded
            }
            
            String data_type = "humid";
            int sensor_id = 1;

            context.Measurements.AddRange(
                new Measurements
                {
                    Data = 0.4m,
                    Timestamp = new DateTime(2022, 12, 29, 23, 2, 1),
                    Data_Type = data_type,
                    Sensor_Id= sensor_id
                },

                new Measurements
                {
                    Data = 0.32m,
                    Timestamp = new DateTime(2022, 12, 30, 0, 32, 1),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                },

                new Measurements
                {
                    Data = 0.2m,
                    Timestamp = new DateTime(2023, 1, 2, 14, 45, 42),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                },

                new Measurements
                {
                    Data = 0.23m,
                    Timestamp = new DateTime(2023, 1, 15, 3, 2, 1),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                },

                new Measurements
                {
                    Data = 0.76m,
                    Timestamp = new DateTime(2023, 2, 11, 23, 2, 1),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                },

                new Measurements
                {
                    Data = 0.63m,
                    Timestamp = new DateTime(2023, 2, 11, 23, 50, 1),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                },

                new Measurements
                {
                    Data = 0.55m,
                    Timestamp = new DateTime(2023, 2, 21, 23, 2, 56),
                    Data_Type = data_type,
                    Sensor_Id = sensor_id
                }
            );
            context.SaveChanges();
        }
    }
}