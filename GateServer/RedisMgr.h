#pragma once
#include "const.h"
#include "hiredis.h"
#include <queue>
#include <atomic>
#include <mutex>
#include "Singleton.h"


class RedisConPool {
public:
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
		: poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
		//����
		for (size_t i = 0; i < poolSize_; ++i) {
			auto* context = redisConnect(host, port);
			if (context == nullptr || context->err != 0) {
				if (context != nullptr) {
					redisFree(context);
				}
				continue;
			}
			//��֤
			auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
			if (reply->type == REDIS_REPLY_ERROR) {
				std::cout << "��֤ʧ��" << std::endl;
				//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
				freeReplyObject(reply);
				redisFree(context);
				continue;
			}
			//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
			freeReplyObject(reply);
			std::cout << "��֤�ɹ�" << std::endl;
			connections_.push(context);
		}
	}
	~RedisConPool() {
		std::lock_guard<std::mutex> lock(mutex_);
		while (!connections_.empty()) {
			connections_.pop();
		}
	}
	redisContext* getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] {
			if (b_stop_) {
				return true;
			}
			return !connections_.empty();
			});
		//���ֹͣ��ֱ�ӷ��ؿ�ָ��
		if (b_stop_) {
			return  nullptr;
		}
		auto* context = connections_.front();
		connections_.pop();
		return context;
	}
	void returnConnection(redisContext* context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(context);
		cond_.notify_one();
	}
	void Close() {
		b_stop_ = true;
		cond_.notify_all();//�����������ڹ���ȴ����ӵ��߳�
	}
private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char* host_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};


class RedisMgr: public Singleton<RedisMgr>, 
	public std::enable_shared_from_this<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	//bool Connect(const std::string& host, int port);//�ڳ����Ѿ���װ�����Ӳ���
	bool Get(const std::string& key, std::string& value);//��ȡkey��Ӧ��value
	bool Set(const std::string& key, const std::string& value);//����key��value
	bool Auth(const std::string& password);//������֤
	bool LPush(const std::string& key, const std::string& value);//���push
	bool LPop(const std::string& key, std::string& value);//���pop
	bool RPush(const std::string& key, const std::string& value);//�Ҳ�push
	bool RPop(const std::string& key, std::string& value);//�Ҳ�pop
	bool HSet(const std::string& key, const std::string& hkey, const std::string& value);//���ö���key��ֵ
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);//�������������
	std::string HGet(const std::string& key, const std::string& hkey);//��ȡ����key��ֵ
	bool Del(const std::string& key);//ɾ������
	bool ExistsKey(const std::string& key);//�жϼ�ֵ�Ƿ����
	void Close();//�ر�


private:
	RedisMgr();
	std::unique_ptr<RedisConPool> _con_pool;
};

