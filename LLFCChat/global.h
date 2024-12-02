﻿#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QStringConverter>
#include <QDir>
#include <QSettings>

/**
 * @brief repolish 用于刷新qss
 */
extern std::function<void(QWidget*)> repolish;

/**
 * 异或字符串，用于密码存储时加密
 */
extern std::function<QString(QString)> xorString;

enum RegId{
    ID_GET_VERIFY_CODE = 1001,//验证码功能id
    ID_REG_USER = 1002,//注册用户功能id
	ID_RESET_PWD = 1003,//重置密码
	ID_LOGIN_USER = 1004,//用户登录
	ID_CHAT_LOGIN = 1005,//登陆聊天服务器
	ID_CHAT_LOGIN_RSP = 1006,//登录聊天服务器回包
};

enum Modules {
	REGISTERMOD = 0,//注册模块
	RESETMOD = 1,//重置模块
	LOGINMOD = 2,//登录模块
};

//注册输入内容错误码
enum TipErr {
	TIP_SUCCESS = 0,
	TIP_EMAIL_ERR = 1,
	TIP_PWD_ERR = 2,
	TIP_CONFIRM_ERR = 3,
	TIP_PWD_CONFIRM = 4,
	TIP_VARIFY_ERR = 5,
	TIP_USER_ERR = 6
};

//错误码
enum ErrorCodes{
    SUCCESS = 0,//成功
    ERR_JSON = 1,//json解析失败
    ERR_NETWORK = 2,//网络错误
};

//鼠标状态
enum ClickLbState {
	Normal = 0,//普通状态
	Selected = 1//选中状态
};

extern QString gate_url_prefix;

struct ServerInfo {
	QString Host;
	QString Port;
	QString Token;
	int Uid;
};

//聊天界面几种模式
enum ChatUIMode {
	SearchMode, //搜索模式
	ChatMode, //聊天模式
	ContactMode, //联系人模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType {
	CHAT_USER_ITEM, //聊天用户
	CONTACT_USER_ITEM, //联系人用户
	SEARCH_USER_ITEM, //搜索到的用户
	ADD_USER_TIP_ITEM, //提示添加用户
	INVALID_ITEM,  //不可点击条目
	GROUP_TIP_ITEM, //分组提示条目
	LINE_ITEM,  //分割线
	APPLY_FRIEND_ITEM, //好友申请
};
#endif // GLOBAL_H
