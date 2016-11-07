/*
* Robot_Manager.h
*
 *  Created on: Jan 4, 2016
 *      Author: zhangyalei
*/

#ifndef ROBOT_MANAGER_H_
#define ROBOT_MANAGER_H_

#include "Thread.h"
#include "List.h"
#include "Bit_Buffer.h"
#include "Buffer_List.h"
#include "Object_Pool.h"
#include "Robot_Connector.h"
#include "Robot.h"

class Robot_Manager: public Thread {
	typedef Object_Pool<Connector, Mutex_Lock> Connector_Pool;
	typedef Object_Pool<Robot> Robot_Pool;
	typedef Buffer_List<Mutex_Lock> Data_List;
	typedef List<int, Mutex_Lock> Int_List;
	typedef std::unordered_map<int, Robot *> Cid_Robot_Map;
public:
	static Robot_Manager *instance(void);
	static void destroy(void);
	void run_handler(void);

	int init(void);

	int process_list();
	int process_buffer(Byte_Buffer &buffer);

	inline int push_buffer(Byte_Buffer *buffer) { return buffer_list_.push_back(buffer); }
	inline void push_tick(int tick) { tick_list_.push_back(tick); }

	int tick(void);
	int login_tick(Time_Value &now);
	int robot_tick(Time_Value &now);

	Robot *connect_center(const char *account = nullptr);
	int connect_gate(int center_cid, const char* gate_ip, int gate_port, std::string& token, std::string& account);

	int send_to_center(int cid, int msg_id, Bit_Buffer &buffer);
	int send_to_gate(int cid, int msg_id, Bit_Buffer &buffer);

	Robot* get_robot(int cid);

	inline Time_Value &server_tick() { return server_tick_; };
	inline int send_msg_interval() { return send_msg_interval_; }
	inline int robot_lifetime() { return robot_lifetime_; }
	inline int robot_mode() { return robot_mode_; }

private:
	Robot_Manager(void);
	virtual ~Robot_Manager(void);
	int print_report(void);

private:
	static Robot_Manager *instance_;

	Connector_Pool connector_pool_;
	Robot_Pool robot_pool_;

	Connector *center_connector_;
	Connector *gate_connector_;

	std::string center_ip_;		//center服务器ip
	int center_port_;					//center服务器端口
	int robot_count_;					//机器人数量
	int login_interval_;				//机器人登录间隔(毫秒)
	int send_msg_interval_;		//发送数据间隔,单位是毫秒
	int robot_lifetime_;    	//机器人运行时间,单位是秒
	int robot_mode_;						//机器人模式，0是自动化模式，1是交互模式
	int robot_index_;

	Time_Value server_tick_;
	Time_Value first_login_tick_;
	Time_Value last_login_tick_;

	Data_List buffer_list_;					//消息列表
	Int_List tick_list_;

	Cid_Robot_Map robot_map_;
};

#define ROBOT_MANAGER Robot_Manager::instance()

#endif /* ROBOT_MANAGER_H_ */
