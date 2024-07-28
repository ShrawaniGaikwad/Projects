import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

public class ClientGUI {
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                new ClientLogic();
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }
}

class ClientLogic extends Frame implements ActionListener, WindowListener {
    public Frame fobj;
    public Label l1, l2,l4;
    public TextArea messageArea, messageArea2;
    public TextField tobj1;
    public Button sendButton;
    private PrintStream ps;
    private BufferedReader br;

    public ClientLogic() throws Exception {
        // Create the main frame
        fobj = new Frame("Chat Application - Client");
        fobj.setLayout(null);
        fobj.addWindowListener(this);
        fobj.setSize(1600, 900);
        fobj.setBackground(Color.PINK);
        fobj.setVisible(true);

        // Create a panel with a border to hold all components
        JPanel panel = new JPanel();
        panel.setLayout(null);
        panel.setBounds(400, 40, 800, 750);
        panel.setBackground(Color.LIGHT_GRAY);
        panel.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2));
        fobj.add(panel);

        // Create and add labels to the panel
        l1 = new Label("Welcome to the Chat Application!");
        l1.setBounds(250, 40, 400, 40);
        l1.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l1);

        l2 = new Label("Client application is running");
        l2.setBounds(250, 100, 400, 40);
        l2.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l2);

        l4 = new Label("");
        l4.setBounds(250, 160, 400, 40);
        l4.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l4);
        
        // Create and add a text area for received messages
        messageArea = new TextArea();
        messageArea.setBounds(50, 220, 700, 200);
        messageArea.setFont(new Font("Tahoma", Font.PLAIN, 18));
        messageArea.setEditable(false);
        panel.add(messageArea);

        // Create and add a text area for sent messages
        messageArea2 = new TextArea();
        messageArea2.setBounds(50, 440, 700, 200);
        messageArea2.setFont(new Font("Tahoma", Font.PLAIN, 18));
        messageArea2.setEditable(false);
        panel.add(messageArea2);

        // Create and add text field and send button
        tobj1 = new TextField();
        tobj1.setBounds(50, 660, 500, 40);
        panel.add(tobj1);

        sendButton = new Button("Send");
        sendButton.setBackground(Color.GREEN);
        sendButton.setBounds(560, 660, 100, 40);
        sendButton.addActionListener(this);
        panel.add(sendButton);

        // Start client in a separate thread
        Thread clientThread = new Thread(this::runClient);
        clientThread.start();
    }

    private void runClient() {
        String s1;
        try {
            Socket s = new Socket("localhost", 1100);
            BufferedReader br = new BufferedReader(new InputStreamReader(s.getInputStream()));
            ps = new PrintStream(s.getOutputStream());

            // Listen for messages from the server
            while ((s1 = br.readLine()) != null) {
                final String serverMessage = s1;
                SwingUtilities.invokeLater(() -> {
                    System.out.println("Server Says: " + serverMessage);
                    messageArea.append("Server Says: " + serverMessage + "\n");
                });

                // Wait for the user to input a message and press send
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

            s.close();
            br.close();
            ps.close();
        } catch (Exception eobj) {
            System.out.println(eobj);
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == sendButton) {
            l4.setText("Type Your Message : ");
            String s3 = tobj1.getText();
            ps.println(s3);
            messageArea2.append("Sent: " + s3 + "\n");
            tobj1.setText("");

            // Notify the client thread that a message has been sent
            synchronized (this) {
                this.notify();
            }
        }
    }

    // Implementing WindowListener methods (not all methods are used)
    public void windowClosing(WindowEvent wobj) {
        System.exit(0);
    }

    public void windowOpened(WindowEvent wobj) {}

    public void windowActivated(WindowEvent wobj) {}

    public void windowDeactivated(WindowEvent wobj) {}

    public void windowIconified(WindowEvent wobj) {}

    public void windowDeiconified(WindowEvent wobj) {}

    public void windowClosed(WindowEvent wobj) {}
}
