/*
	Copyright (C) 2003 - 2022
	by David White <dave@whitevine.net>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#pragma once

class config;
class config_writer;
class unit;
#include <string>
#include <map>
#include <vector>

namespace statistics
{
	struct stats
	{
		stats();
		explicit stats(const config& cfg);

		config write() const;
		void write(config_writer &out) const;
		void read(const config& cfg);

		typedef std::map<std::string,int> str_int_map;
		str_int_map recruits, recalls, advanced_to, deaths, killed;
		int recruit_cost, recall_cost;

		/*
		 *  A type that will map a string of hit/miss to the number of times
		 *  that sequence has occurred.
		 */
		typedef str_int_map battle_sequence_frequency_map;

		/** A type that will map different % chances to hit to different results. */
		typedef std::map<int,battle_sequence_frequency_map> battle_result_map;

		/** Statistics of this side's attacks on its own turns. */
		battle_result_map attacks_inflicted;
		/** Statistics of this side's attacks on enemies' turns. */
		battle_result_map defends_inflicted;
		/** Statistics of enemies' counter attacks on this side's turns. */
		battle_result_map attacks_taken;
		/** Statistics of enemies' attacks against this side on their turns. */
		battle_result_map defends_taken;

		long long damage_inflicted, damage_taken;
		long long turn_damage_inflicted, turn_damage_taken;

		struct hitrate_t
		{
			int strikes; //< Number of strike attempts at the given CTH
			int hits; //< Number of strikes that hit at the given CTH
			hitrate_t() = default;
			explicit hitrate_t(const config& cfg);
			config write() const;
		};
		/** A type that maps chance-to-hit percentage to number of hits and strikes at that CTH. */
		typedef std::map<int, hitrate_t> hitrate_map;
		hitrate_map by_cth_inflicted, by_cth_taken;
		hitrate_map turn_by_cth_inflicted, turn_by_cth_taken;

		static const int decimal_shift = 1000;

		// Expected value for damage inflicted/taken * 1000, based on
		// probability to hit,
		// Use this long term to see how lucky a side is.

		long long expected_damage_inflicted, expected_damage_taken;
		long long turn_expected_damage_inflicted, turn_expected_damage_taken;
		std::string save_id;
	};

	int sum_str_int_map(const std::map<std::string,int>& m);
	int sum_cost_str_int_map(const std::map<std::string,int>& m);

	struct scenario_context
	{
		scenario_context(const std::string& name);
		~scenario_context();
	};

	struct attack_context
	{
		attack_context(const unit& a, const unit& d, int a_cth, int d_cth);
		~attack_context();

		enum hit_result { MISSES, HITS, KILLS };

		void attack_expected_damage(double attacker_inflict, double defender_inflict);
		void attack_result(hit_result res, int cth, int damage, int drain);
		void defend_result(hit_result res, int cth, int damage, int drain);

	private:

		std::string attacker_type, defender_type;
		std::string attacker_side, defender_side;
		int chance_to_hit_defender, chance_to_hit_attacker;
		std::string attacker_res, defender_res;

		stats& attacker_stats();
		stats& defender_stats();
	};

	void recruit_unit(const unit& u);
	void recall_unit(const unit& u);
	void un_recall_unit(const unit& u);
	void un_recruit_unit(const unit& u);
	int un_recall_unit_cost(const unit& u);

	void advance_unit(const unit& u);

	config write_stats();
	void write_stats(config_writer &out);
	void read_stats(const config& cfg);
	void fresh_stats();
	/** Delete the current scenario from the stats. */
	void clear_current_scenario();
	/** Reset the stats of the current scenario to the beginning. */
	void reset_current_scenario();

	void reset_turn_stats(const std::string & save_id);
	stats calculate_stats(const std::string & save_id);
	/** Stats (and name) for each scenario. The pointers are never nullptr. */
	typedef std::vector< std::pair<const std::string *, const stats *>> levels;
	/** Returns a list of names and stats for each scenario in the current campaign. */
	levels level_stats(const std::string & save_id);
} // end namespace statistics
std::ostream& operator<<(std::ostream& outstream, const statistics::stats::hitrate_t& by_cth);
