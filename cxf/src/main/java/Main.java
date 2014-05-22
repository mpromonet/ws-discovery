import java.util.List;
import javax.xml.ws.EndpointReference;
import org.apache.cxf.ws.discovery.WSDiscoveryClient;

public class Main 
{
	public static void main(String[] args) 
	{
		WSDiscoveryClient client = new WSDiscoveryClient();
		client.setVersion10(); // use WS-discovery 1.0
		client.setDefaultProbeTimeout(1000); // timeout 1s
		
		System.out.println("Probe:" + client.getAddress());
		List<EndpointReference> references = client.probe();
		
		System.out.println("Nb answsers:" + references.size());
		for (EndpointReference ref : references)
		{
			System.out.println(ref.toString());
		}
	}
}
