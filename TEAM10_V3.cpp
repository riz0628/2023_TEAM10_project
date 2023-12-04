#include <time.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include "openai.hpp"

using namespace std;
using json = nlohmann::json;

class interviewer{
	public:
		virtual void check_pass(string ans){};
		virtual void print_pass(int index){
			if(pass[index]){
				std::cout << "Your answer is safe in terms of harassment.\n";
			}
			else{
				std::cout << "Your answer is not safe in terms of harassment. You must change your answer!\n";
			}
		}

		
	protected:				
		std::vector<bool> pass;
};

class harassment_interviewer: public interviewer{
	public:
		void check_pass(string ans){
			openai::start();
			json answer;
			answer["input"] = ans;
			auto moderation = openai::moderation().create(answer);
			auto response = moderation["results"][0]["categories"]["harassment"].template get<bool>();
			if(response == true)
				pass.push_back(false);
			else
				pass.push_back(true);
		}
		void print_pass(int index){
			if(pass[index]){
				std::cout << "Your answer is safe in terms of harassment.\n";
			}
			else{
				std::cout << "Your answer is not safe in terms of harassment. You must change your answer!\n";
			}
		}
};

class self_harm_interviewer: public interviewer{
	public:
		void check_pass(string ans){
			openai::start();
			json answer;
			answer["input"] = ans;
			auto moderation = openai::moderation().create(answer);
			auto response = moderation["results"][0]["categories"]["self-harm"].template get<bool>();
			if(response == true)
				pass.push_back(false);
			else
				pass.push_back(true);
		}
		void print_pass(int index){
			if(pass[index]){
				std::cout << "Your answer is safe in terms of self_harm.\n";
			}
			else{
				std::cout << "Your answer is not safe in terms of self_harm. You must change your answer!\n";
			}
		}
};

class violence_interviewer: public interviewer{
	public:
		void check_pass(string ans){
			openai::start();
			json answer;
			answer["input"] = ans;
			auto moderation = openai::moderation().create(answer);
			auto response = moderation["results"][0]["categories"]["violence"].template get<bool>();
			if(response == true)
				pass.push_back(false);
			else
				pass.push_back(true);
		}
		void print_pass(int index){
			if(pass[index]){
				std::cout << "Your answer is safe in terms of violence.\n";
			}
			else{
				std::cout << "Your answer is not safe in terms of violence. You must change your answer!\n";
			}
		}
};

class master_interviewer: public interviewer{
	public:
		void set_topic(){
			cout << "Before we start the interview, write the number of topics you want to set for the interview. For default topics, '0' is fine.\n";
			cin >> topic_num;
			getchar();
			if(topic_num == 0){
				topic_list.push_back("job-related knowledge");
				topic_list.push_back("job-related competencies");
				topic_list.push_back("problem-solving ability");
				topic_list.push_back("decision-making ability");
				topic_list.push_back("leadership skills");
				topic_list.push_back("communication skills");
				topic_list.push_back("global competence");
				topic_num = 7;
			}
			else{
				for(int i = 0; i < topic_num; i++){
					std::cout << "write '" << i+1 << "'th topic : ";
					std::string t;
					std::getline(cin,t);
					topic_list.push_back(t);
				}
				
			}
			for(int i = 0; i < topic_num; i++){
				topic_done.push_back(false);
			}
			
		}


		void make_topic_question(int i){
			openai::start(); //
			int topic_index = rand() % topic_num;
			std::string topic;
			if(topic_done[topic_index]){
				topic = ".";
			}
			else{
				topic = " about '" + topic_list[topic_index] + "'.";
			}
			if(i == 0){
				question.push_back("Can you introduce yourself?");
			}
			else{
				string q = "You are an executive interviewer at Samsung Electronics. Ask the interviewee a question" + topic;
				json quest;
				quest["messages"][0]["content"] = q;
				quest["messages"][0]["role"] = "user";
				quest["model"] = "gpt-3.5-turbo";
				quest["temperature"] = 1;
				auto chat = openai::chat().create(quest);
				q = chat["choices"][0]["message"]["content"].template get<std::string>();

				question.push_back(q);
				topic_done[topic_index] = 1;
			}
		}


		void make_tail_question(){//꼬리 질문 만드는 함수
			openai::start(); //
			string q = "In an executive interview with Samsung Electronics, I heard the question '" + question[question.size() - 1] + "', and I answered '" + ans[ans.size() - 1] + "'. Ask me one next question.";
			json quest;
			quest["messages"][0]["content"] = q;
			quest["messages"][0]["role"] = "user";
			quest["model"] = "gpt-3.5-turbo";
			quest["temperature"] = 1;
			auto chat = openai::chat().create(quest);
			q = chat["choices"][0]["message"]["content"].template get<std::string>();
			question.push_back(q);
		}


		void make_comment(){//comment 만드는 함수
			openai::start(); //
			string c = "In an executive interview with Samsung Electronics, I heard the question '" + question[question.size() - 1] + "', and I answered '" + ans[ans.size() - 1] + "'. What do you think this answer is out of 100 and why? Reply me less than four sentences.";
			json quest;
			quest["messages"][0]["content"] = c;
			quest["messages"][0]["role"] = "user";
			quest["model"] = "gpt-3.5-turbo";
			quest["temperature"] = 1;
			auto chat = openai::chat().create(quest);
			c = chat["choices"][0]["message"]["content"].template get<std::string>();
			comment.push_back(c);
		}


		void feedback(interviewer* assistant[]){//질문-답변-comment 출력 && assistant들이 critical한 인성항목에 위배되는 질문인지 아닌지 체크해줌
			for(int i = 0; i < question.size(); i++){
				cout << "Q. " << question[i] << endl << "A. " << ans[i] << "\n***Comment***\n" << comment[i] << endl;
				for(int j = 0; j < 3; j++){
					assistant[j]->check_pass(ans[i]);
					assistant[j]->print_pass(i);
					cout << endl;
				}
			}
		}



		void do_interview(interviewer* assistant[]){//모의면접 실행 함수
			cout << "Hello. This is a mock interview program to prepare for Samsung Electronics' executive interview.\nI hope this program will help you prepare for the interview.\n\n";						
			set_topic();//면접에 사용할 topic 세팅(default or 자율적으로 세팅) : topic_list, topic_num 설정
			for(int i = 0; i <= topic_num; i++){//질문-답변받기-comment 작성
				make_topic_question(i);//토픽 첫 질문 만들기 : 질문 만들고 question에 push_back(), topic_done 갱신
				cout << question[question.size() - 1] << std::endl;//토픽 질문 출력
				string a;//질문 답변 입력받고 ans에 저장
				getline(cin, a);
				ans.push_back(a);
				make_comment();//답변에 대한 comment 만들기

				int tail_num = rand() % 3;//꼬리 질문 갯수
				for(int j = 0; j < tail_num; j++){
					make_tail_question();//꼬리 질문 만들기 : 질문 만들고 question에 push back()
					cout << endl;
					cout << question[question.size() - 1] << endl;//꼬리 질문 출력
					string a;//질문 답변 입력받고 ans에 저장
					getline(cin, a);
					ans.push_back(a);
					make_comment();//답변에 대한 comment 만들기
				}
			}
			feedback(assistant);//피드백 받기
			std::cout << "\n\nNow, the mock interview is over. I hope this mock interview helped you prepare for your interview. Goodbye~";//마무리말 출력
		}




	private:
		std::vector<string> question, ans, comment;//질문, 답변, 코멘트(+ 점수) 저장용
		std::vector<string> topic_list;//면접에서 사용할 topic list
		std::vector<bool> topic_done;//면접에서 해당 topic 이미 사용했는지 안했는지 확인 용도
		int topic_num;//topic_list에 몇개의 topic 저장되어있는가
};

int main() {
    openai::start();
	srand(time(NULL));
	interviewer* assistant[3];
	assistant[0] = new harassment_interviewer();
	assistant[1] = new self_harm_interviewer();
	assistant[2] = new violence_interviewer();
	master_interviewer KK;
	KK.do_interview(assistant);
	//delete assistant[0];
	//delete assistant[1];
	//delete assistant[2];
	return 0;
}
