using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.CompilerServices;

namespace clientApp
{
    //class that handle communication with the server
    public class ClientCommunicator
    {
        private TcpClient _client;
        private NetworkStream _stream;

        private readonly object _lock = new object(); // For thread safety

        public ClientCommunicator(string ip, int port)
        {
            //client that connect to server with port and ip
            _client = new TcpClient(ip, port);  
            _stream = _client.GetStream();
            _stream.ReadTimeout = 5000;  // 5 seconds
            _stream.WriteTimeout = 5000;
        }

        //this func sends a given arr of bytes to the server
        public void Send(byte[] data)
        {
            lock (_lock)
            {
                _stream.Write(data, 0, data.Length);
            }
        }

        //this func receive data from the server
        public byte[] Receive()
        {
            lock (_lock)
            {
                try
                {
                    byte[] header = new byte[5];  // first 5 bytes: 1 for type, 4 for length
                    int totalRead = 0;
                    // reading exactly 5 bytes of the header
                    while (totalRead < 5)
                    {
                        int read = _stream.Read(header, totalRead, 5 - totalRead);
                        if (read == 0) throw new Exception("Disconnected while reading header.");
                        totalRead += read;
                    }
                    //getting the length of the msg
                    int len = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(header, 1));
                    byte[] data = new byte[len];  

                    totalRead = 0;
                    //reading msg to the data arr
                    while (totalRead < len)
                    {
                        int read = _stream.Read(data, totalRead, len - totalRead);
                        if (read == 0) throw new Exception("Disconnected while reading data.");
                        totalRead += read;
                    }
                    //returning the msg from the server where byte1 = code, 2-5 = msg length, the rest = data
                    return header.Concat(data).ToArray();
                }
                catch (IOException ex)
                {
                    if (ex.InnerException is SocketException sockEx && sockEx.SocketErrorCode == SocketError.TimedOut)
                    {
                        throw new TimeoutException("Receive timed out.");
                    }
                    throw;
                }
            }
        }


        //this func close the socket with the server
        public void Close()
        {
            lock (_lock)
            {
                _stream?.Close();
                _client?.Close();
            }
        }
    }
}

