package Server;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import Client.*;
public class server {

    private static ServerSocket serverSocket;
    private static Socket clientSocket = null;
    public static int count_down=0;
    public static int count_up=0;

    public static void main(String[] args) throws IOException {

        try {
            serverSocket = new ServerSocket(25444);
            System.out.println("Đã khởi động server.");
        } catch (Exception e) {
            System.err.println("Cổng đã được sử dụng.");
            System.exit(1);
        }
        while (true) {
            try {
                clientSocket = serverSocket.accept();
                System.out.println("Chấp nhận kết nối : " + clientSocket);
                Thread t = new Thread(new MultiClient(clientSocket));
                t.start();
            } catch (Exception e) {
                System.err.println("Lỗi kết nối.");
            }
        }
    }
};

class MultiClient extends Thread{
    private Socket socket;
    DataInputStream in;
    DataOutputStream out;
    public static final int SIZE_BUFF = 4096;
    public MultiClient(Socket socket){
        this.socket=socket;
    }
    public void run(){
        try {
            in=new DataInputStream(socket.getInputStream());
            String clientSelection;
            while ((clientSelection = in.readLine()) != null) {
                switch (clientSelection) {
                    case "1":
                        continue;
                    case "2":
                        String outGoingFileName;
                        while ((outGoingFileName = in.readLine()) != null) {
                            sendFile("src/Server/SharedFolder/"+outGoingFileName);
                            server.count_down++;
                            System.out.println("Tong file download: "+server.count_down);
                        }

                        continue;
                    case "3":
                        String outGoingFileName1;
                        while ((outGoingFileName1 = in.readLine()) != null) {
                            saveFile(this.socket,"src/Server/"+outGoingFileName1);
                            server.count_up++;
                            System.out.println("Tong file upload: "+server.count_up);
                        }

                        continue;
                    case "4":
                        System.exit(1);
                        break;
                    default:
                        System.out.println("Lỗi biên nhận.");
                        break;
                }

            }

        } catch (IOException ex) {
            Logger.getLogger(MultiClient.class.getName()).log(Level.SEVERE, null, ex);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    public void sendFile(String fileName) {
        try {
            File files = new File(fileName);
            byte[] countbyte = new byte[(int) files.length()];
            FileInputStream fis = new FileInputStream(files);
            BufferedInputStream bis = new BufferedInputStream(fis);
            DataInputStream dis = new DataInputStream(bis);
            dis.readFully(countbyte, 0, countbyte.length);
            OutputStream os = socket.getOutputStream();
            DataOutputStream dos = new DataOutputStream(os);
            dos.writeUTF(files.getName());
            dos.writeLong(countbyte.length);
            dos.write(countbyte, 0, countbyte.length);
            dos.flush();
            System.out.println("File "+fileName+" sent to client.");
        } catch (Exception e) {
            System.err.println("File không tồn tại!");
        }
    }
    public void saveFile(Socket clientSocket, String fileName) throws Exception {
        ObjectOutputStream oos = new ObjectOutputStream(clientSocket.getOutputStream());
        ObjectInputStream  ois= new ObjectInputStream(clientSocket.getInputStream());
        FileOutputStream fos = new FileOutputStream(new File(fileName));
        Object obj = ois.readObject();
        byte[] buffer = new byte[SIZE_BUFF];
        if (obj instanceof String) {
            fos = new FileOutputStream(obj.toString());
        } else {
            throw new Exception("Error read file!");
        }
        Integer bytesRead = 0;
        do {
            obj = ois.readObject();
            if (!(obj instanceof Integer)) {
                throw new Exception("Error!");
            }
            bytesRead = (Integer)obj;
            obj = ois.readObject();
            if (!(obj instanceof byte[])) {
                throw new Exception("Error!");
            }
            buffer = (byte[])obj;
            fos.write(buffer, 0, bytesRead);

        } while (bytesRead == SIZE_BUFF);
        System.out.println("Save file success.");
    }
}