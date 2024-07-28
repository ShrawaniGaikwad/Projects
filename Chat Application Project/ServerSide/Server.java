package ServerSide;
import java.lang.*;
import java.net.*;
import java.io.*;

public class Server
{
    public void server() throws Exception
    {
        System.out.println("Server application is running");                               //prints when server is started
        String s1,s2;                                                                      //creates two string s1 and s2

        ServerSocket ss = new ServerSocket(1100);                                          //Creates a server socket which is to be connected to client
        Socket s = ss.accept();                                                            //Gets connection from client

        System.out.println("Connection successfull");                                      //prints if client is connected
        BufferedReader brK = new BufferedReader(new InputStreamReader(System.in));         //Creates input stream from keyboard
        BufferedReader br = new BufferedReader(new InputStreamReader(s.getInputStream())); //created input stream to read from socket i.eclient

        PrintStream ps = new PrintStream(s.getOutputStream());                             //for sending response to the client     
        while((s1 = br.readLine()) != null)                                                 //reading from client
        {
            System.out.println("Client Says : " +s1);                                       //prints message recieved from client
            System.out.println("Enter message to be send to the client : ");                
            s2 = brK.readLine();                                                            //reading from keyboard
            ps.println(s2);                                                                 //send to the client
        }

        s.close();
        ss.close();
        brK.close();
        br.close();
        ps.close();
    }
}