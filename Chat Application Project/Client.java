import java.lang.*;
import java.io.*;
import java.net.*;

class Client
{
    public static void main(String args[]) throws Exception
    {
        System.out.println("Client application is running");
        String s1,s2;

        Socket s = new Socket("localhost",1100);

        BufferedReader brK = new BufferedReader(new InputStreamReader(System.in));
        BufferedReader br = new BufferedReader(new InputStreamReader(s.getInputStream()));

        PrintStream ps = new PrintStream((s.getOutputStream()));

        System.out.println("Enter the message to be send to the server");
        while(!(s1 = brK.readLine()).equals("gn"))
        {
            ps.println(s1);
            s2 = br.readLine();
            System.out.println("Server says : "+s2);
            System.out.println("Enter the message to be send to the server");

        }

    }
}