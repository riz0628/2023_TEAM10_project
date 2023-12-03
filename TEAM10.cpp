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
		void set_harassment_result(string result)
		{
			if(result == "PASS")
				harassment_result = 1;
			else
				harassment_result = 0;
			
		}
		void set_self_harm_result(string result)
                {
                        if(result == "PASS")
                                self_harm_result = 1;
                        else
                                self_harm_result = 0;

                }
		void set_violence_result(string result)
                {
                        if(result == "PASS")
                                violence_result = 1;
                        else
                                violence_result = 0;

                }

		string get_Name(){return name;}
		int get_harassment(){return harassment_result;}
		int get_self_harm(){return self_harm_result;}
		int get_violence(){return violence_result;}
        private:
                int harassment_result; // 0 = FAIL. 1 = PASS.
		int self_harm_result; // 0 = FAIL. 1 = PASS.
		int violence_result; // 0 = FAIL. 1 = PASS.
		string name;


};

class Interviewer  		//PARENT CLASS
{
	public:
		Interviewer() : score(0){}
		float get_score() {return score;}
		void set_score(float new_score){score = new_score;}
		virtual float evaluation(string answer){ return score;}  		//Virtual function for Polymorphism , DONT'T CARE
	private:
		float score;

};

class Harassment_Interviewer: public Interviewer		// CHILD CLASS 1
{
        public:
                Harassment_Interviewer() : Interviewer(), harassment("PASS"){}
     		void set_result(float cur_score){
			if(cur_score <= 0.2 && harassment == "PASS")
				harassment = "PASS";
			else
				harassment  = "FAIL";
		}
		string get_result(){return harassment;}

		virtual float evaluation(string answer){	// RETURN HARASSMENT SCORE FOR YOUR ANSWER
        		openai::start();	
                	json answer1;
                	answer1["input"] = answer;
                	auto moderation = openai::moderation().create(answer1);
                	auto response = moderation["results"][0]["category_scores"]["harassment"].template get<float>();
                	return response;
		}
        private:
		string harassment;  // if score is 0~0.2 "PASS" else "FAIL"

};

class self_harm_Interviewer: public Interviewer                // CHILD CLASS 2
{
        public:
                self_harm_Interviewer() : Interviewer(), self_harm("PASS"){}
                void set_result(float cur_score){
                        if(cur_score <= 0.2 && self_harm == "PASS")
                                self_harm = "PASS";
                        else
                                self_harm  = "FAIL";
                }
                string get_result(){return self_harm;}

                virtual float evaluation(string answer){ 	// RETURN SELF-HARM SCORE FOR YOUR ANSWER
                        openai::start();
                        json answer1;
                        answer1["input"] = answer;
                        auto moderation = openai::moderation().create(answer1);
                        auto response = moderation["results"][0]["category_scores"]["self-harm"].template get<float>();
			return response;
                }
        private:
                string self_harm;  // if score is 0~0.2 "PASS" else "FAIL"

};

class violence_Interviewer: public Interviewer                // CHILD CLASS 3
{
        public:
                violence_Interviewer() : Interviewer(), violence("PASS"){}
                void set_result(float cur_score){
                        if(cur_score <= 0.2 && violence == "PASS")
                                violence = "PASS";
                        else
                                violence  = "FAIL";
                }
                string get_result(){return violence;}

                virtual float evaluation(string answer){ // RETURN VIOLENCE SCORE FOR YOUR ANSWER
                        openai::start();
                        json answer1;
                        answer1["input"] = answer;
                        auto moderation = openai::moderation().create(answer1);
                        auto response = moderation["results"][0]["category_scores"]["violence"].template get<float>();
			return response;
                }
        private:
                string violence;  // if score is 0~0.2 "PASS" else "FAIL"

};
// Function that make QUESTION BY GPT !!
string make_question(){
	openai::start();
	{
		auto chat = openai::chat().create(R"(
    		{
        		"model": "gpt-3.5-turbo",
        		"messages":[{"role":"user", "content":"Ask me one question that can simply assess my personality in one sentence."}],
        		"temperature": 2
    		}
    		)"_json);
		auto response = chat["choices"][0]["message"]["content"].template get<std::string>();
		return response;
	}
}

// Function that Evaluate your answer!!
/*float evaluation(string answer){
	openai::start();
	{
		json answer1;
		answer1["input"] = answer;
		auto moderation = openai::moderation().create(answer1);
		auto response = moderation["results"][0]["category_scores"]["harassment"].template get<float>();
		return response;

	}
}*/


int main() {
	openai::start();
	{
		Interviewee YOU;
		Harassment_Interviewer I1;
		self_harm_Interviewer I2;
		violence_Interviewer I3;
		string question;
		string answer;
		string temp;


		cout << "Hello, what is your name?" << endl;
		getline(cin, temp);
		YOU.set_Name(temp);
		cout << "OK, " << YOU.get_Name() << " let's start interview" << endl;
		for(int i = 1; i < 4; i++)		//ASK 3 QUESTION.
		{
			cout <<"Q"<< i << " : " << make_question() << endl;
			getline(cin, answer);
			I1.set_result(I1.evaluation(answer));
			cout << I1.evaluation(answer) << endl; // VIEW HARASSMENT SCORE (FOR DEBUGGING)
			I2.set_result(I2.evaluation(answer));
			cout << I2.evaluation(answer) << endl; // VIEW SELF-HARM SCORE (FOR DEBUGGING)
			I3.set_result(I3.evaluation(answer));
			cout << I3.evaluation(answer) << endl; // VIEW VIOLENCE SCORE (FOR DEBUGGING)
			YOU.set_harassment_result(I1.get_result());
			YOU.set_self_harm_result(I2.get_result());
			YOU.set_violence_result(I3.get_result());
		}
		cout << "Interview is Done!" << endl;
		if(YOU.get_harassment() && YOU.get_self_harm() && YOU.get_violence())
			cout << "CONGRATULATION YOU HAVE PASSED INTERVIEW!!" << endl;
		else
			cout << "SORRY, YOU HAVE FAILED INTERVIEW" << endl;
	}
 
    return 0;
}
