using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GPIBlibrary;
using System.Threading;
using System.IO;
using System.Globalization;
using System.Numerics;
using System.IO.Ports;

namespace ConsoleApplication4
{
    class Program
    {
        static void Main(string[] args)
        {
            
            NumberFormatInfo nfi = new CultureInfo("en-US", false).NumberFormat;
            Console.WriteLine("Select port for the USB GPIB ADAPTER");
            string portret = chooseSerialPort();
            GPIB bus = new GPIB();
            bus.start(portret, 5000);

            Console.WriteLine("Select port for the Freezer");
            string freezerport = chooseSerialPort();
            Console.WriteLine("Input file name without .txt");
            string filename = Console.ReadLine();
            System.IO.StreamWriter myfile = new StreamWriter(AppDomain.CurrentDomain.BaseDirectory.ToString()+filename + ".txt", true);
            Console.WriteLine("All inputs are in Kelvins, K = C + 273.15");
            Console.WriteLine("Input the low temprature in K");
            int lowtemp = int.Parse(Console.ReadLine(), System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture);

            Console.WriteLine("Input the high temprature in K");
            int hightemp = int.Parse(Console.ReadLine(), System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture);

            Console.WriteLine("Input the number of items measured at each temprature: ");
            int noofboards = int.Parse(Console.ReadLine(), System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture);

            Console.WriteLine("Input the number data points at each temprature: ");
            int datapoints = int.Parse(Console.ReadLine(), System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture);

            SerialPort freezer = new SerialPort();
            freezer.PortName = freezerport;
            freezer.BaudRate = 9600;
            freezer.DataBits = 8;
            freezer.Parity = Parity.None;
            freezer.StopBits = StopBits.One;
            freezer.ReadBufferSize = 1048576;

            Console.WriteLine("Starting ramp-down");
            // time to start to soak the freezer at the temp
            bool ramp = true;
            freezer.Open();
            Thread.Sleep(new TimeSpan(0, 0, 5));
            int templowtemp = lowtemp - 3;
            freezer.WriteLine("K" + templowtemp.ToString());
            string currenttemp = "";
            while (ramp == true) {
                freezer.WriteLine("T");
              currenttemp = freezer.ReadLine();
              Console.WriteLine("temp:   " + currenttemp);
              if (float.Parse(currenttemp, System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture) <= lowtemp)
              {
                ramp = false;
            }
            Thread.Sleep(new TimeSpan(0,0,30)); // report temp once each min

        }

           Console.WriteLine("going into 60min soak at " + currenttemp+"   at  "+DateTime.Now.ToString("t"));
           Thread.Sleep(new TimeSpan(1, 0, 0)); // change soak time here

           Console.WriteLine("Starting measurement at " + DateTime.Now.ToString("t"));
            myfile.WriteLine("Starting measurement at "+DateTime.Now.ToString("t"));

      //      bool measure = true;
            freezer.WriteLine("T");
              currenttemp = freezer.ReadLine();
              
        

        for(int i = lowtemp; i < hightemp; i++)
        { 
            freezer.WriteLine("K"+(i).ToString());

            

                for (int j = 1; j <= datapoints; j++)
                { // could have some conditional logic to select between lots or little data

                    // Thread.Sleep(new TimeSpan(0, 5, 0));
                    Thread.Sleep(new TimeSpan(0, 0, 5));
                    freezer.WriteLine("T");
                    currenttemp = freezer.ReadLine();
                    string[] freqarray = new String[6];
                    for (int k = 1; k <= noofboards; k++)
                    {
                        bus.write(26,"A"+k.ToString());
                        Thread.Sleep(new TimeSpan(0, 0, 2));
                    bus.address(2);
                    bus.write(2, "FN3GT3MD2");
                    string freq = bus.writeread(2, "MR");
                    freq = freq.Remove(0, 5);
                    freq = freq.TrimEnd('\r', '\n');
                    freqarray[k] = freq;
                    }
                    myfile.WriteLine(float.Parse(currenttemp, System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture).ToString("000.00", CultureInfo.InvariantCulture) + ";" + freqarray[1] + ";" + freqarray[2] + ";" + freqarray[3] + ";" + freqarray[4] + ";" + freqarray[5]);
                    Console.WriteLine(float.Parse(currenttemp, System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture).ToString("000.00", CultureInfo.InvariantCulture) + "       " + ";" + freqarray[1] + ";" + freqarray[2] + ";" + freqarray[3] + ";" + freqarray[4] + freqarray[5] + "   " + i.ToString());
                    // i = (int)float.Parse(currenttemp, System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture);
                    myfile.Flush();
                
            }
        }
        Console.WriteLine("Program end");
          
           freezer.WriteLine("C20");
           Console.WriteLine("Stopping measurement at " + DateTime.Now.ToString("t"));
            myfile.Close();
            Console.ReadLine();
            
        }

        static string chooseSerialPort()
        {
            string[] ports = SerialPort.GetPortNames();
            int i = 1;
            foreach (string portname in ports)
            {
                Console.WriteLine(i.ToString() + "   " + portname);
                i++;
            }
            Console.WriteLine("Write the number of the selected port:");
            i = Convert.ToInt16(Console.ReadLine());
            return ports[i - 1];
        }
    }
}
