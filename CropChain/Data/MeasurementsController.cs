using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using static System.Net.Mime.MediaTypeNames;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace CropChain.Data
{
    [Route("api/[controller]")]
    [ApiController]
    public class MeasurementsController : ControllerBase
    {
        private readonly CropChainContext _context;

        public MeasurementsController(CropChainContext context)
        {
            _context = context;
        }

        // GET: api/Measurements
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Measurements>>> GetMeasurements()
        {
          if (_context.Measurements == null)
          {
              return NotFound();
          }
            return await _context.Measurements.ToListAsync();
        }

        // GET: api/Measurements/lastn
        [HttpGet("lastn")]
        public async Task<ActionResult<IEnumerable<Measurements>>> GetNMeasurements(int n)
        {
            if (_context.Measurements == null)
            {
                return NotFound();
            }

            List<Measurements> measurements = _context.Measurements.OrderByDescending(x => x.Timestamp).Take(n).ToList();

            return await _context.Measurements.OrderByDescending(x => x.Timestamp).Take(n).ToListAsync();
        }

        // GET: api/Measurements/5
        [HttpGet("{id}")]
        public async Task<ActionResult<Measurements>> GetMeasurements(int id)
        {
          if (_context.Measurements == null)
          {
              return NotFound();
          }
            var measurements = await _context.Measurements.FindAsync(id);

            if (measurements == null)
            {
                return NotFound();
            }

            return measurements;
        }

        // PUT: api/Measurements/5
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPut("{id}")]
        public async Task<IActionResult> PutMeasurements(int id, Measurements measurements)
        {
            if (id != measurements.Id)
            {
                return BadRequest();
            }

            _context.Entry(measurements).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!MeasurementsExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/Measurements
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<Measurements>> PostMeasurements(Measurements measurements)
        {
          if (_context.Measurements == null)
          {
              return Problem("Entity set 'CropChainContext.Measurements'  is null.");
          }
            _context.Measurements.Add(measurements);
            await _context.SaveChangesAsync();

            return CreatedAtAction("GetMeasurements", new { id = measurements.Id }, measurements);
        }

        // DELETE: api/Measurements/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteMeasurements(int id)
        {
            if (_context.Measurements == null)
            {
                return NotFound();
            }
            var measurements = await _context.Measurements.FindAsync(id);
            if (measurements == null)
            {
                return NotFound();
            }

            _context.Measurements.Remove(measurements);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool MeasurementsExists(int id)
        {
            return (_context.Measurements?.Any(e => e.Id == id)).GetValueOrDefault();
        }
    }
}
