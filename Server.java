import java.io.*;
import java.net.*;

class Server
{
    public static void main(String Arg[]) throws Exception
    {
        System.out.println("Server of Marvellous Chat Messanger is running...");
        ServerSocket ssobj=new ServerSocket(2100);
        System.out.println("server is in the listening mode at port number:2100");

        Socket sobj = ssobj.accept();
        System.out.println("Client and Server connection is successful...");

        PrintStream ps = new PrintStream(sobj.getOutputStream());

        BufferedReader br1 = new BufferedReader(new InputStreamReader(sobj.getInputStream()));
        BufferedReader br2 = new BufferedReader(new InputStreamReader(System.in));

        System.out.println("Marvellous chat messanger started");

        String str1,str2;

        while((str1 = br1.readLine())!=null)
        {
            System.out.println("Client says :"+str1);
            System.out.println("Enter the meassage for client");
            str2=br2.readLine();
            ps.println(str2);
        } 
        System.out.println("Thanking for using the Chat Meassanger..");  
        ssobj.close();
        sobj.close();
        ps.close();
        br1.close();
        br2.close();

    }
}