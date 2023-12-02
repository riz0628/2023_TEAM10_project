#include "openai.hpp"
#include <iostream>
#include <string>
using namespace std;
using json = nlohmann::json;

class Interviewee
{
        public:
                Interviewee() :harassment_result(){};
		void set_Name(string new_name){name = new_name;}
		void set_harassment_result(int result){harassment_result = result;}
		void set_self_harm_result(int result){self_harm_result = result;}
		void set_violence_result(int result){violence_result = result;}
		string get_Name(){return name;}
        private:
                int harassment_result; // 0 = FAIL. 1 = PASS.
		int self_harm_result; // 0 = FAIL. 1 = PASS.
		int violence_result; // 0 = FAIL. 1 = PASS.
		string name;


};

class Interviewer
{
	public:
		Interviewer() : score(0){}
		float get_score() {return score;}
		void set_score(float new_score){score = new_score;}
	private:
		float score;

};

class Harassment_Interviewer: public Interviewer
{
        public:
                Harassment_Interviewer() : harassment("FAIL"){}
     		void set_evaluation(float cur_score){
			if(cur_score <= 0.3)
				harassment = "PASS";
			else
				harassment  = "FAIL";
		}
		string get_evaluation(){return harassment;}
        private:
		string harassment;  // if score is 0~0.3 "PASS" else "FAIL"

};
// Function that make QUESTION BY GPT !!
string make_question(){
	openai::start();
	{
		//string question = "Ask questions that can simply assess personality.";
		//json question1;
		//question1["input"] = question;	
		//auto chat = openai::chat().create(question1);
		auto chat = openai::chat().create(R"(
    		{
        		"model": "gpt-3.5-turbo",
        		"messages":[{"role":"user", "content":"Ask me one questions that can simply assess my personality in one sentence."}],
        		"temperature": 1
    		}
    		)"_json);
		auto response = chat["choices"][0]["message"]["content"].template get<std::string>();
		return response;
	}
}

// Function that Evaluate your answer!!
float evaluation(string answer){
	openai::start();
	{
		json answer1;
		answer1["input"] = answer;
		auto moderation = openai::moderation().create(answer1);
		auto response = moderation["results"][0]["category_scores"]["harassment"].template get<float>();
		return response;

	}
}


int main() {
	openai::start();
	{
		Interviewee YOU;
		Harassment_Interviewer I1;
		string question;
		string answer;
		string temp;
		json answer1;


		cout << "Hello, what is your name?" << endl;
		getline(cin, temp);
		YOU.set_Name(temp);
		cout << "OK, " << YOU.get_Name() << " let's start interview" << endl;
		cout << make_question() << endl;
		getline(cin, answer);
		I1.set_evaluation(evaluation(answer));
		cout << "YOUR HARASSMENT SCORE IS :" << evaluation(answer) << " || YOU HAVE " << I1.get_evaluation() << "ED HARASSMENT TEST" << endl;
	}
 
    return 0;
}

