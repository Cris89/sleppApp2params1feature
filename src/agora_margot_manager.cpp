#include "agora_margot_manager.hpp"

Agora_Margot_Manager::Agora_Margot_Manager()
{
	
}

void Agora_Margot_Manager::init()
{
	std::string appName = "sleepApp2params1feature";

	int numMetrics = 2;

	// in this case, 100 is a feature value
	std::vector<int> features_indexes = { 0 };



	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////// good_func
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// the order of parameters and features must be lexicographic
	// es. parameters: "aaa" and "ccc", features: "bbb" --> aaa = 150, bbb = 360, ccc = 100
	std::vector<float> defaultConfiguration = { 100, 150, 360 };

	// metrics and parameters must follow a lexicographic order
	std::vector< std::string > info = { "metric error",
										"metric throughput",

										"param param1 enum 1 50 150 300 450 700 800",
										"param param2 range 10 850 70",

										"numFeats 1",
										"minNumObsFeatValues 3",

										"DoE fcccd",
										"lhdSamples 10",
										"numReps 1",

										"RSM sparkGenLinRegrPolyComb2" };

	// milliseconds
	int threadMQTTReqSleepTime = 3000;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////// good_func
	////////////////////////////////////////////////////////////////////////////////////////////////////



	/*////////////////////////////////////////////////////////////////////////////////////////////////////
	////////// strange_func
	////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<float> defaultConfiguration = {15, 10, 10};

	std::vector< std::string > info = { "metric avg_error",
										"metric avg_throughput",

										"param param1 enum 1 10 15 25 40 65 80",
										"param param2 enum 1 5 10 20",
										"param param3 range 10 46 3",

										"doe fcccd",
										
										"lhdSamples 10",

										"numOPs 1",

										"rsm sparkGenLinRegr2nd" };

	// milliseconds
	int threadMQTTReqSleepTime = 3000;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////// strange_func
	////////////////////////////////////////////////////////////////////////////////////////////////////*/
	




	agora_framework = new Framework( appName,

										numMetrics,

										defaultConfiguration,
										features_indexes,
										
										info,
											
										threadMQTTReqSleepTime );

	agora_framework->init();





	operating_points_t defaultOP = agora_framework->getAppStruct()->getOperatingPoints()->getNewOPs();

	margot::sleeping::manager.add_operating_points( defaultOP );

	margot::init();
}

void Agora_Margot_Manager::updateOPs()
{
	agora_framework->checkOPs();

	if(agora_framework->changeOPs == true)
	{
		operating_points_t newOPs = agora_framework->getAppStruct()->getOperatingPoints()->getNewOPs();
		operating_points_t currentOPs = agora_framework->getAppStruct()->getOperatingPoints()->getCurrentOPs();
		operating_points_t commonOPs = agora_framework->getAppStruct()->getOperatingPoints()->getCommonOPs();

		/*printf("\t+-------newOPs---------+\n");
		for( auto newOP : newOPs)
		{
			for ( const auto c : newOP.first )
			{
				printf("\t|% 22.2f|\n", c);
			}

			for ( const auto c : newOP.second )
			{
				printf("\t|% 22.2f|\n", c);
			}

			printf("\t+----------------------+\n");
		}

		printf("\n\t+-----currentOPs-------+\n");
		for( auto currentOP : currentOPs)
		{
			for ( const auto c : currentOP.first )
			{
				printf("\t|% 22.2f|\n", c);
			}

			for ( const auto c : currentOP.second )
			{
				printf("\t|% 22.2f|\n", c);
			}

			printf("\t+----------------------+\n");
		}

		printf("\n\t+-----commonOPs--------+\n");
		for( auto commonOP : commonOPs)
		{
			for ( const auto c : commonOP.first )
			{
				printf("\t|% 22.2f|\n", c);
			}

			for ( const auto c : commonOP.second )
			{
				printf("\t|% 22.2f|\n", c);
			}

			printf("\t+----------------------+\n");
		}*/

		margot::sleeping::manager.add_operating_points(newOPs);
		margot::sleeping::manager.remove_operating_points(currentOPs);
		margot::sleeping::manager.add_operating_points(commonOPs);

		// margot::sleeping::manager.dump();

		agora_framework->updateOPs();
	}
}

// the order of parameters and features in params_features must be lexicographic
// the order of metrics in metrics must be lexicographic
void Agora_Margot_Manager::sendResult( std::vector<float> params_features, std::vector<float> metrics )
{
	std::string operatingPoint;

	std::vector<int> fs_idx = agora_framework->getAppStruct()->getFeaturesIndexes();

	std::vector<float> ps_v;
	std::vector<float> fs_v;



	const int num_params_features = params_features.size();
	for( int i = 0; i < num_params_features; ++i )
	{
		bool is_feature = false;
		
		for( const int index : fs_idx )
		{
			if( i == index )
			{
				is_feature = true;
				break;
			}
		}
		
		if( is_feature )
		{
			fs_v.emplace_back( params_features[i] );
		}
		
		else
		{
			ps_v.emplace_back( params_features[i] );
		}
	}



	const int num_params = ps_v.size();
	for( int i = 0; i < num_params; i++ )
	{
		operatingPoint += std::to_string( ps_v[i] ) + " ";
	}

	operatingPoint = operatingPoint.substr( 0, operatingPoint.size() - 1 );
	operatingPoint += ":";

	const int num_feats = fs_v.size();
	for( int i = 0; i < num_feats; i++ )
	{
		operatingPoint += std::to_string( fs_v[i] ) + " ";
	}

	operatingPoint = operatingPoint.substr( 0, operatingPoint.size() - 1 );
	operatingPoint += ":";

	const int num_metrics = metrics.size();
	for( int i = 0; i < num_metrics; i++ )
	{
		operatingPoint += std::to_string( metrics[i] ) + " ";
	}

	operatingPoint = operatingPoint.substr( 0, operatingPoint.size() - 1 );

	agora_framework->sendResult( operatingPoint );
}

// features must be in lexicographic order
void Agora_Margot_Manager::storeFeatures( std::vector<float> features )
{
	agora_framework->storeFeatures( features );
}

Agora_Margot_Manager::~Agora_Margot_Manager()
{

}