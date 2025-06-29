using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using Newtonsoft.Json;
//this class is helper to the communication with the server
public class CommunicationHelper
{
    //this function gets the code of req, and the data and build the buffer of bytes
    // 1 byte - the code, 4 next bytes - the data length, the rest - the data
    public static byte[] BuildMessage(byte code, object data)
    {
        //convering data to json
        string json = JsonConvert.SerializeObject(data);
        //converting json to bytes
        byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
        //getting the data length
        int length = jsonBytes.Length;
        //converting length to bytes
        byte[] lenBytes = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(length));

        //initializing a new list of bytes
        List<byte> buffer = new List<byte>();
        buffer.Add(code);  //adding first byte as code
        buffer.AddRange(lenBytes);  //adding the length bytes
        buffer.AddRange(jsonBytes);  //adding the data bytes

        return buffer.ToArray();  //returning the list bytes as array
    }

    //this function convert arr of bytes to the code of the msg, and the json msg
    public static (byte code, string json) ParseMessage(byte[] buffer)
    {
        byte code = buffer[0];  //getting the code
        //getting the length of the data
        int len = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer, 1));
        //getting the data and keep it in json string
        string json = Encoding.UTF8.GetString(buffer, 5, len);
        return (code, json);  //returning the code and the json string
    }
}
