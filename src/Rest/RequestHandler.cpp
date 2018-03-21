/**
 * \brief Ce fichier contient l'implémentation de méthodes
 *        de la classe RequestHandler.
 * \date 10/03/18
 * \author jordy
 */

#include "../../include/Rest/RequestHandler.hpp"
#include "../../include/Exception.hpp"

// API de persistence
#include "../../include/API/PersistenceAPI.hpp"
#include "../../include/API/AccountAPI.hpp"
#include "../../include/API/PersonAPI.hpp"
#include "../../include/API/SessionAPI.hpp"

#include "../../include/Util/Converter/Converters.hpp"

#include <exception>
#include <plog/Log.h>

// api JSON
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

// STL
#include <memory>
#include <ctime>

using namespace Entity ;
using namespace API ;
using namespace Util ;

namespace RestAPI
{
    void RequestHandler::getAccountById(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        long id ;
        try
        {
            id = request.param(":id").as<int>() ;
        }catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
            return ;
        }
        // Pas d'erreur on continue
        bool flag = true ;
        std::string json ;
        std::shared_ptr<Account> account ;
        {
            try
            {
                /* voyons voir si le comptes est courant ou non */
                account = AccountAPI::getInstance()->findById<CurrentAccount>(id) ;
            }catch(const NotFound &nf)
            {
                flag = false ;
            }
        }
        // Il n'y a pas de compte courant avec cette ID, on essaie
        // avec un compte d'épargne.
        if(!flag)
        {
            try
            {
                /* voyons voir si le compte est courant ou non */
                account = AccountAPI::getInstance()->findById<SavingsAccount>(id) ;
            }catch(const NotFound &nf)
            {
                LOG_ERROR << nf.what() ;
                response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
                return ;
            }
            // Il s'agit d'un compte d'épargne. On envoie une réponse.
            json = SavingsAccountConverter().entityToJson(dynamic_cast<SavingsAccount*>(account.get()));
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
            return ;
        }
        // Il s'agit d'un compte courant j'envoie la bonne réponse
        json = CurrentAccountConverter().entityToJson(dynamic_cast<CurrentAccount*>(account.get()));
        response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
    }

    void RequestHandler::getAccountByCustomerId(const Rest::Request &request, Http::ResponseWriter response)
    {
        try
        {
            long id = request.param(":id").as<int>() ;
            std::vector<std::shared_ptr<Account> > accounts = AccountAPI::getInstance()->findByCustomerId<Account>(id) ;
            std::string json("[\n\t") ;
            for(auto &account : accounts)
            {
                json += AccountConverter().entityToJson(account)+",\n";
            }
            json.pop_back() ;
            json.pop_back() ;
            json += "\t]\n";
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
        catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
        }
    }

    void RequestHandler::getAccountByEmployeeId(const Rest::Request &request, Http::ResponseWriter response)
    {
        try
        {
            long id = request.param(":id").as<int>() ;
            std::vector<std::shared_ptr<Account> > accounts = AccountAPI::getInstance()->findByEmployeeId<Account>(id) ;
            std::string json("[\n\t") ;
            for(auto &account : accounts)
            {
                json += AccountConverter().entityToJson(account)+",\n";
            }
            json.pop_back() ;
            json.pop_back() ;
            json += "\t]\n";
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
        catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
        }
    }

    void RequestHandler::getEmployeeById(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        try
        {
            long id = request.param(":id").as<int>() ;
            std::shared_ptr<Employee> person = PersonAPI::getInstance()->findById<Employee>(id) ;
            std::string json = EmployeeConverter().entityToJson(person.get());
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
        catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
        }
    }

    void RequestHandler::getAllEmployees(const Rest::Request &request, Http::ResponseWriter response)
    {
        try
        {
            std::vector<std::shared_ptr<Employee> > employees = PersonAPI::getInstance()->findAll<Employee>() ;
            std::string json("[\n\t") ;
            for(auto &employee : employees)
            {
                json += EmployeeConverter().entityToJson(employee)+",\n";
            }
            json.pop_back() ;
            json.pop_back() ;
            json += "\t]\n";
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
    }

    void RequestHandler::getCustomerById(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        try
        {
            long id = request.param(":value").as<int>() ;
            std::shared_ptr<Customer> person = PersonAPI::getInstance()->findById<Customer>(id) ;
            std::string json = CustomerConverter().entityToJson(person.get());
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
        catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
        }
    }

    void RequestHandler::getAllCustomers(const Rest::Request &request, Http::ResponseWriter response)
    {
        try
        {
            std::vector<std::shared_ptr<Customer> > customers = PersonAPI::getInstance()->findAll<Customer>() ;
            std::string json("[\n\t") ;
            for(auto &customer : customers)
            {
                json += CustomerConverter().entityToJson(customer)+",\n";
            }
            json.pop_back() ;
            json.pop_back() ;
            json += "\t]\n";
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
    }

    void RequestHandler::getCustomerByName(const Rest::Request &request, Http::ResponseWriter response)
    {
    	try
        {
            std::string name_pattern = request.param(":value").as<std::string>() ;
            std::vector<std::shared_ptr<Customer> > customers = PersonAPI::getInstance()->findByName<Customer>(name_pattern) ;
            CustomerConverter cc ;
            std::string json("[\n\t") ;
            for(auto &customer : customers)
            {
                json += cc.entityToJson(customer)+",\n";
            }
            json.pop_back() ;
            json.pop_back() ;
            json += "\t]\n";
            response.send(Http::Code::Ok, json, MIME(Application, Json)) ;

        }catch(const NotFound &nf)
        {
            LOG_ERROR << nf.what() ;
            response.send(Http::Code::Not_Found, nf.what(), MIME(Text, Plain)) ;
        }
        catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), MIME(Text, Plain)) ;
        }
    }

    // Les routes DELETE
    void RequestHandler::deletePerson(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        long id ;
        try
        {
            id = request.param(":id").as<int>() ;
        }catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
            return ;
        }
        // Pas d'erreur on continue
        bool flag = true ;
        {
            try
            {
                /* voyons voir si la personne est un client */
                PersonAPI::getInstance()->erase<Customer>(id) ;
            }catch(const NotErasable &ne)
            {
                flag = false ;
            }
        }
        // Il n'y a pas de client avec cette ID, on essaie
        // avec un Employee.
        if(!flag)
        {
            try
            {
                /* voyons voir si la personne est un client */
                PersonAPI::getInstance()->erase<Employee>(id) ;
            }catch(const NotErasable &ne)
            {
                LOG_ERROR << ne.what() ;
                response.send(Http::Code::Not_Found, ne.what(), MIME(Text, Plain)) ;
                return ;
            }
        }
        // peut importe qu'il s'agissent d'un client ou d'un employee
        response.send(Http::Code::Ok) ;
    }

    void RequestHandler::deleteAccount(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        long id ;
        try
        {
            id = request.param(":id").as<int>() ;
        }catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
            return ;
        }
        // Pas d'erreur on continue
        bool flag = true ;
        {
            try
            {
                /* voyons voir si le compte est courant ou pas */
                PersonAPI::getInstance()->erase<CurrentAccount>(id) ;
            }catch(const NotErasable &ne)
            {
                flag = false ;
            }
        }
        // Il n'y a pas de compte courant avec cette ID, on essaie
        // avec un compte d'épargne.
        if(!flag)
        {
            try
            {
                /* voyons voir si le compte est d'épargne */
                PersonAPI::getInstance()->erase<SavingsAccount>(id) ;
            }catch(const NotErasable &ne)
            {
                LOG_ERROR << ne.what() ;
                response.send(Http::Code::Not_Found, ne.what(), MIME(Text, Plain)) ;
                return ;
            }
        }
        // peut importe qu'il s'agissent d'un compte courant ou d'épargne
        response.send(Http::Code::Ok) ;
    }

    void RequestHandler::deleteOperation(const Rest::Request &request, Http::ResponseWriter response)
    {
        /* Vérifions le paramettre */
        long id ;
        try
        {
            id = request.param(":id").as<int>() ;
        }catch(const std::exception &e)
        {
            LOG_ERROR << e.what() ;
            response.send(Http::Code::Not_Acceptable, e.what(), Http::Mime::MediaType::fromString("text/plain")) ;
            return ;
        }
        // Pas d'erreur on continue
        bool flag = true ;
        {
            try
            {
                /* voyons voir si l'opération est un virement ou pas' */
                PersonAPI::getInstance()->erase<Virement>(id) ;
            }catch(const NotErasable &ne)
            {
                flag = false ;
            }
        }
        // Il n'y a pas de virement avec cette ID, on essaie
        // avec une opération simple.
        if(!flag)
        {
            try
            {
                /* voyons voir si l'opération est une opération simple */
                PersonAPI::getInstance()->erase<Employee>(id) ;
            }catch(const NotErasable &ne)
            {
                LOG_ERROR << ne.what() ;
                response.send(Http::Code::Not_Found, ne.what(), MIME(Text, Plain)) ;
                return ;
            }
        }
        response.send(Http::Code::Ok) ;
    }

    // Les routes POST
    void RequestHandler::addCustomer(const Rest::Request &request, Http::ResponseWriter response)
    {
    	response.send(Http::Code::Ok, "Tout est OK", Http::Mime::MediaType::fromString("text/plain"));
    }

    void RequestHandler::addEmployee(const Rest::Request &request, Http::ResponseWriter response)
    {
    	response.send(Http::Code::Ok, "Tout est OK", Http::Mime::MediaType::fromString("text/plain"));
    }

    void RequestHandler::addAccount(const Rest::Request &request, Http::ResponseWriter response)
    {
    	response.send(Http::Code::Ok, "Tout est OK", Http::Mime::MediaType::fromString("text/plain"));
    }

    void RequestHandler::addOperation(const Rest::Request &request, Http::ResponseWriter response)
    {
    	response.send(Http::Code::Ok, "Tout est OK", Http::Mime::MediaType::fromString("text/plain"));
    }

    // Les routes pour la connexion
    void RequestHandler::authentification(const Rest::Request &request, Http::ResponseWriter response)
    {
        std::shared_ptr<Session> session ;
        try
        {
            rapidjson::Document doc ;
            if(doc.Parse(request.body().c_str()).HasParseError())
            {
                LOG_INFO << request.body() ;
                response.send(Http::Code::Bad_Request) ;
                return ;
            }
            if(!doc.HasMember("passwd") || !doc.HasMember("email"))
            {
                LOG_INFO << request.body() ;
                response.send(Http::Code::Bad_Request) ;
                return ;
            }
            rapidjson::Value &passwd = doc["passwd"] ;
            rapidjson::Value &email = doc["email"] ;
            // Récupérons la person avec les credentials
            std::shared_ptr<Person> person = PersonAPI::getInstance()
                                                ->findByCredentials<Person>(
                                                    std::string(email.GetString()), std::string(passwd.GetString())) ;
            // On récupère sa session
            session = SessionAPI::getInstance()->findById<Session>(person->getId()) ;
            // On vérifie la validité de la session grâce au timestamp courant
            std::time_t current_time = std::time(nullptr) ;
            ulong timestamp = (ulong) current_time ;
            if(session->getEnd() < current_time)
            {
                std::string err_msg = "" ;
                response.send(Http::Code::Unauthorized, err_msg, MIME(Application, Json)) ;
                return ;
            }
            // Envoie du jeton
            std::string msg = "{\"token\":\""+session->getToken()+"\"}" ;
            response.send(Http::Code::Ok, msg, MIME(Application, Json)) ;
        }
        catch(const NotFound &nf)
        {
            LOG_WARNING << "Tentative frauduleuse de connexion !" ;
            response.send(Http::Code::Unauthorized) ;
            return ;
        }
    	response.send(Http::Code::Ok, session->getToken(), Http::Mime::MediaType::fromString("text/plain"));
    }

    // La route pour l'inscription
    void RequestHandler::subscription(const Rest::Request &request, Http::ResponseWriter response)
    {
        response.send(Http::Code::Ok, request.param("?token").name(), Http::Mime::MediaType::fromString("text/plain"));
    }
}