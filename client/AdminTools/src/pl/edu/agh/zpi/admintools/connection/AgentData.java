package pl.edu.agh.zpi.admintools.connection;

public class AgentData {
	private short id;
	private float temp;
	
	public AgentData() {
	}
	
	public AgentData(short id, float temp) {
		this.id = id;
		this.temp = temp;
	}
	
	@Override
	public String toString(){
		return "id="+id+" temp="+temp; 
	}
	

}
